// ====================================================================
// DESIGN STANDARDS: RT Thread-Safe Lock-Free Memory Subsystem
// Components: Pre-allocated concurrent fixed-block vertex matrix pool
// ====================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <atomic>
#include "ConcurrentVertexMemoryPool.generated.h"

// Define strict upper limits for high-performance matrix chunk chunks
#define VERTEX_POOL_BLOCK_SIZE 4096
#define VERTEX_POOL_MAX_BLOCKS 64

/**
 * Encapsulates a fixed-capacity structural block allocated cleanly on initialization.
 */
struct FVertexMemoryBlock
{
    FVector MatrixBuffer[VERTEX_POOL_BLOCK_SIZE];
    int32 ActiveElementCount;

    FVertexMemoryBlock() : ActiveElementCount(0) {}
    
    void Reset()
    {
        ActiveElementCount = 0;
    }
};

/**
 * Thread-safe concurrent memory manager utilizing atomic indexing
 * to completely eliminate mutex locking overhead during parallel ticking operations.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THEBRAINASABIOMASSREACTOR_API UConcurrentVertexMemoryPool : public UActorComponent
{
    GENERATED_BODY()

public:
    UConcurrentVertexMemoryPool();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    /**
     * Leases a pre-allocated fixed-block container concurrently from the pool.
     * Returns nullptr if all memory blocks are currently active across worker threads.
     */
    FVertexMemoryBlock* LeaseBlock();

    /**
     * Safely returns an allocated block back to the concurrent stack.
     */
    void RecycleBlock(FVertexMemoryBlock* BlockToReturn);

private:
    // Fixed structural block array allocated as a single continuous memory block
    FVertexMemoryBlock* MasterMemoryPool;

    // Lock-free circular index stacks managed entirely via standard atomics
    FVertexMemoryBlock* AvailableBlocksStack[VERTEX_POOL_MAX_BLOCKS];
    std::atomic<int32> TopIndex;
};
