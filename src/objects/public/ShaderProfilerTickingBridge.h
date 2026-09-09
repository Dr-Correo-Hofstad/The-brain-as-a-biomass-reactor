// ====================================================================
// DESIGN STANDARDS: RT Multi-Threaded Real-Time Shader Profile Bridge
// Components: Hardware Render Query Tracker & Async Material Parameter Pump
// ====================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ConcurrentVertexMemoryPool.h"
#include "RHI.h"
#include "RenderResource.h"
#include "ShaderProfilerTickingBridge.generated.h"

/**
 * Stores real-time hardware telemetry captured from the GPU command queue.
 */
USTRUCT(BlueprintType)
struct FGPUProfileTelemetry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RT Telemetry | GPU Profiling")
    int32 ExecutionTimeMicroseconds;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RT Telemetry | GPU Profiling")
    bool bTelemetryValid;

    FGPUProfileTelemetry()
        : ExecutionTimeMicroseconds(0)
        , bTelemetryValid(false)
    {}
};

/**
 * Thread-safe ticking manager that reads vertex matrices from the concurrent pool, 
 * dispatches them to GPU parameter registers, and benchmarks the hardware execution time.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THEBRAINASABIOMASSREACTOR_API UShaderProfilerTickingBridge : public UActorComponent
{
    GENERATED_BODY()

public:
    UShaderProfilerTickingBridge();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    /** Enqueues an active vertex block out to the target material shader parameter lines */
    UFUNCTION(BlueprintCallable, Category = "RT Shader Pipeline | Bridge")
    void PushMatrixUpdateAsync(FVertexMemoryBlock* InDataBlock, UMaterialInstanceDynamic* InTargetMaterial);

    /** Current verified hardware telemetry metrics */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RT Shader Pipeline | Telemetry")
    FGPUProfileTelemetry LatestTelemetry;

private:
    // Low-level RHI render query checkpoints for measuring microsecond execution spans
    FRenderQueryRHIRef TimestampStartQuery;
    FRenderQueryRHIRef TimestampEndQuery;

    // Local tracking configurations
    TArray<FLinearColor> GPUVectorBuffer;
    int32 MaxBufferElements;
    bool bQueryIssued;
};
