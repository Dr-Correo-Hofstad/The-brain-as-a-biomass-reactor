#include "ShaderVertexBridge.h"

UShaderVertexBridge::UShaderVertexBridge()
{
    PrimaryComponentTick.bCanEverTick = false; // Synchronous event-driven updates driven by async callbacks
    MaxShaderArrayElements = 128; // Standard optimization envelope matching material array parameter limits
}

void UShaderVertexBridge::BeginPlay()
{
    Super::BeginPlay();
    
    // Pre-allocate the vector array buffer to completely avoid runtime allocations on the main render pipeline
    PackedShaderVectors.Empty();
    PackedShaderVectors.AddZeroed(MaxShaderArrayElements);
}

void UShaderVertexBridge::PipeVertexArrayToShader(FVertexMemoryBlock* ProcessedBlock, UMaterialInstanceDynamic* TargetMaterialInstance)
{
    // 1. Boundary Protection Check
    if (!ProcessedBlock || !TargetMaterialInstance)
    {
        return;
    }

    // Determine safe transmission volume based on active elements and shader hard-caps
    int32 ElementsToPipe = FMath::Min(ProcessedBlock->ActiveElementCount, MaxShaderArrayElements);

    // 2. Linearize and Pack 3D Vector Offsets into 4-Float Channels (RGBA / XYZ-Scale)
    for (int32 i = 0; i < ElementsToPipe; ++i)
    {
        const FVector& VertexCoord = ProcessedBlock->MatrixBuffer[i];
        
        // Map Cartesian X, Y, Z directly to RGB vector lanes inside the shader engine
        PackedShaderVectors[i] = FLinearColor(
            VertexCoord.X,
            VertexCoord.Y,
            VertexCoord.Z,
            1.0f // Scale/Weight factor channel
        );
    }

    // 3. Clear Remaining Slots to Prevent Artifacts or Memory Leaks
    for (int32 i = ElementsToPipe; i < MaxShaderArrayElements; ++i)
    {
        PackedShaderVectors[i] = FLinearColor::Black;
    }

    // 4. Force Direct Ingestion into the Dynamic Material Shader Parameter State
    // The target material must contain a Vector Parameter named 'DeformationVectorArray' configured as an array node
    TargetMaterialInstance->SetVectorParameterValue(TEXT("DeformationVectorArray"), PackedShaderVectors[0]);
    
    UE_LOG(LogTemp, Log, TEXT("[+] Shader Pipeline Synchronization Complete: Piped %d structural vertex points straight to GPU parameters."), ElementsToPipe);
}
