#include "ConcurrentVertexMemoryPool.h"

UConcurrentVertexMemoryPool::UConcurrentVertexMemoryPool()
{
    PrimaryComponentTick.bCanEverTick = false;
    MasterMemoryPool = nullptr;
    TopIndex.store(-1, std::memory_order_relaxed);
}

void UConcurrentVertexMemoryPool::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("[*] Initializing Pre-Allocated Concurrent Vertex Memory Pool..."));
    
    // 1. Allocate a single continuous chunk of system memory to maximize CPU cache locality
    MasterMemoryPool = new FVertexMemoryBlock[VERTEX_POOL_MAX_BLOCKS];

    // 2. Initialize the atomic pointer tracking stack
    for (int32 i = 0; i < VERTEX_POOL_MAX_BLOCKS; ++i)
    {
        AvailableBlocksStack[i] = &MasterMemoryPool[i];
    }

    // Set the stack pointer index to the highest pre-allocated address slot
    TopIndex.store(VERTEX_POOL_MAX_BLOCKS - 1, std::memory_order_release);
    
    UE_LOG(LogTemp, Log, TEXT("[+] Memory Pool Setup Complete. Pre-allocated Block Budget: %d Matrix Segments."), VERTEX_POOL_MAX_BLOCKS);
}

void UConcurrentVertexMemoryPool::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Clean up continuous heap allocation blocks safely during system teardown
    if (MasterMemoryPool != nullptr)
    {
        delete[] MasterMemoryPool;
        MasterMemoryPool = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

FVertexMemoryBlock* UConcurrentVertexMemoryPool::LeaseBlock()
{
    // Compare-and-Swap Loop (Lock-Free Contention Resolution Strategy)
    int32 CurrentTop = TopIndex.load(std::memory_order_acquire);
    
    while (CurrentTop >= 0)
    {
        int32 NextTop = CurrentTop - 1;
        // Attempt to atomically claim the block index slot
        if (TopIndex.compare_exchange_weak(CurrentTop, NextTop, std::memory_order_release, std::memory_order_acquire))
        {
            FVertexMemoryBlock* LeasedBlock = AvailableBlocksStack[CurrentTop];
            LeasedBlock->Reset(); // Quick reset without heap reclamation passes
            return LeasedBlock;
        }
        // If another background worker thread updated the top index first, the loop automatically retries
    }

    // Pool exhaust edge condition met: all pre-allocated slots are currently leased
    UE_LOG(LogTemp, Warning, TEXT("[!] CRITICAL PERFORMANCE SHUTDOWN: Concurrent Vertex Memory Pool completely exhausted under load!"));
    return nullptr;
}

void UConcurrentVertexMemoryPool::RecycleBlock(FVertexMemoryBlock* BlockToReturn)
{
    if (BlockToReturn == nullptr) return;

    int32 CurrentTop = TopIndex.load(std::memory_order_acquire);
    int32 NextTop = CurrentTop + 1;

    // Verify boundaries to guarantee memory protection bounds are not violated
    if (NextTop >= VERTEX_POOL_MAX_BLOCKS)
    {
        UE_LOG(LogTemp, Error, TEXT("[!] FATAL POOL EXCEPTION: Attempted to recycle more blocks than budgeted memory limits allow."));
        return;
    }

    while (true)
    {
        AvailableBlocksStack[NextTop] = BlockToReturn;
        
        if (TopIndex.compare_exchange_weak(CurrentTop, NextTop, std::memory_order_release, std::memory_order_acquire))
        {
            break;
        }
        // Update local indexing requirements to match concurrent modifications
        NextTop = CurrentTop + 1;
    }
}
