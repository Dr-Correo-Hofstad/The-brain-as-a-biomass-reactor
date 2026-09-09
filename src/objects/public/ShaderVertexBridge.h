// ====================================================================
// DESIGN STANDARDS: RT Material Shader Interface Substep Manager
// Components: Pre-allocated vertex array to GPU parameter pipe
// ====================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ConcurrentVertexMemoryPool.h"
#include "ShaderVertexBridge.generated.h"

/**
 * Manages the high-performance serialization of thread-safe vertex arrays
 * straight into GPU material parameters for real-time vertex shader manipulation.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THEBRAINASABIOMASSREACTOR_API UShaderVertexBridge : public UActorComponent
{
    GENERATED_BODY()

public:
    UShaderVertexBridge();

protected:
    virtual void BeginPlay() override;

public:
    /**
     * Pipes a leased memory block's structural transformations directly into 
     * a targeted Dynamic Material Instance parameter array.
     */
    UFUNCTION(BlueprintCallable, Category = "RT Shader Ingestion | Pipelines")
    void PipeVertexArrayToShader(FVertexMemoryBlock* ProcessedBlock, UMaterialInstanceDynamic* TargetMaterialInstance);

    /** Max number of spatial tracking nodes pushed as an absolute array bound to the shader */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RT Shader Ingestion | Constraints")
    int32 MaxShaderArrayElements;

private:
    // Pre-allocated array storing the formatted vector parameters for GPU dispatch
    TArray<FLinearColor> PackedShaderVectors;
};
