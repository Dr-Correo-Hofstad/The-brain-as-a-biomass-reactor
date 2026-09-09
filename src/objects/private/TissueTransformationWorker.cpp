#include "TissueTransformationWorker.h"

void FTissueTransformationWorker::DoWork()
{
    CalculatedDeformations.Empty();
    CalculatedDeformations.Reserve(InputVertices.Num());

    // Local scaling factor mapped from current internal tissue limits
    float ScalarImpedance = CurrentThresholds.BrainNeuralMatrixMPa * 100.0f;
    float DeflectionLimit = CurrentThresholds.CapillaryBedBoundaryMPa * 50.0f;

    // Parallelized processing loop executing on secondary background CPU core strings
    for (const FVector& BaseVertex : InputVertices)
    {
        FVector TransformedCoordinates = BaseVertex;

        // Apply a deterministic spatiotemporal deformation wave calculation
        float WaveOffset = FMath::Sin(BaseVertex.X * ScalarImpedance + BaseVertex.Y * ScalarImpedance);
        TransformedCoordinates.Z += WaveOffset * DeflectionLimit;

        CalculatedDeformations.Add(TransformedCoordinates);
    }
}

UTissueTransformationPipeline::UTissueTransformationPipeline()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics; // Execute calculations before the physics phase
    bHasPendingData = false;
    CurrentAsyncTask = nullptr;
}

void UTissueTransformationPipeline::BeginPlay()
{
    Super::BeginPlay();

    // Initialize mock source baseline vertex matrix coordinates for testing
    SourceBaselineVertices.Empty();
    for (int32 i = 0; i < 500; ++i)
    {
        SourceBaselineVertices.Add(FVector(FMath::FRandRange(-50.f, 50.f), FMath::FRandRange(-50.f, 50.f), 0.f));
    }
}

void UTissueTransformationPipeline::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Poll the async background task status safely on the main game thread
    if (CurrentAsyncTask && CurrentAsyncTask->IsDone())
    {
        // Safe context switch: copy background thread arrays to local thread-safe buffers
        ThreadSafeVertexOutputs = CurrentAsyncTask->GetTask().CalculatedDeformations;
        bHasPendingData = true;

        delete CurrentAsyncTask;
        CurrentAsyncTask = nullptr;
        
        UE_LOG(LogTemp, Log, TEXT("[+] Async Thread Worker Complete: 3D Structural Transformation Matrix Synced to Main Render Thread."));
    }
}

void UTissueTransformationPipeline::DispatchTransformationTask(const FTissueYieldThresholds& ActiveLimits)
{
    // Block double dispatch cycles if a task is currently active across the thread pool
    if (CurrentAsyncTask != nullptr)
    {
        return;
    }

    bHasPendingData = false;

    // Allocate and dispatch the non-abandonable task to the Unreal Engine thread pool
    CurrentAsyncTask = new FAsyncTask<FTissueTransformationWorker>(ActiveLimits, SourceBaselineVertices);
    CurrentAsyncTask->StartBackgroundTask();
}
