// ====================================================================
// DESIGN STANDARDS: RT Multi-Threaded Physics Engine Substep Manager
// Components: FNonAbandonableTask worker and transformation actor
// ====================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Async/AsyncWork.h"
#include "StateMatrixBridge.h"
#include "TissueTransformationWorker.generated.h"

/**
 * Asynchronous worker task responsible for calculating physical 3D mesh deformation
 * and structural transformation matrices based on current tissue mechanical limits.
 */
class FTissueTransformationWorker : public FNonAbandonableTask
{
    friend class FAsyncTask<FTissueTransformationWorker>;

public:
    FTissueTransformationWorker(const FTissueYieldThresholds& InThresholds, const TArray<FVector>& InBaseVertices)
        : CurrentThresholds(InThresholds)
        , InputVertices(InBaseVertices)
    {}

    FORCEINLINE TStatId GetStatId() const
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(FTissueTransformationWorker, STATGROUP_ThreadPoolAsyncTasks);
    }

    // Core worker execution path executed on the background thread pool
    void DoWork();

    // Outputs stored safely for thread retrieval
    TArray<FVector> CalculatedDeformations;

private:
    FTissueYieldThresholds CurrentThresholds;
    TArray<FVector> InputVertices;
};

/**
 * Multi-threaded ticking actor component that interfaces with the Unreal Engine main loop
 * to safely poll and deploy async computational transformations.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THEBRAINASABIOMASSREACTOR_API UTissueTransformationPipeline : public UActorComponent
{
    GENERATED_BODY()

public:
    UTissueTransformationPipeline();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    /** Triggers a new asynchronous calculation cycle across background workers */
    UFUNCTION(BlueprintCallable, Category = "RT Multi-Threading | Pipelines")
    void DispatchTransformationTask(const FTissueYieldThresholds& ActiveLimits);

    /** Broadcasts true when calculated data maps are ready to be integrated into the rendering pipeline */
    UPROPERTY(BlueprintReadOnly, Category = "RT Multi-Threading | Status")
    bool bHasPendingData;

    /** Thread-safe array storing the latest calculated structural coordinates */
    TArray<FVector> ThreadSafeVertexOutputs;

private:
    TArray<FVector> SourceBaselineVertices;
    FAsyncTask<FTissueTransformationWorker>* CurrentAsyncTask;
};
