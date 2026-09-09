#include "ShaderProfilerTickingBridge.h"
#include "RenderingThread.h"

UShaderProfilerTickingBridge::UShaderProfilerTickingBridge()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics; // Execute telemetry passes directly after physics evaluations
    MaxBufferElements = 128;
    bQueryIssued = false;
}

void UShaderProfilerTickingBridge::BeginPlay()
{
    Super::BeginPlay();
    GPUVectorBuffer.AddZeroed(MaxBufferElements);

    // Initialize the low-level RHI hardware time-stamp query pools on the render thread
    ENQUEUE_RENDER_COMMAND(InitializeRHIQueries)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            if (RHISupportsTimestampRenderQueries())
            {
                TimestampStartQuery = RHICreateRenderQuery(RHT_ResultBuffer);
                TimestampEndQuery = RHICreateRenderQuery(RHT_ResultBuffer);
            }
        });
}

void UShaderProfilerTickingBridge::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Release RHI resource boundaries safely during component teardown
    TimestampStartQuery.SafeRelease();
    TimestampEndQuery.SafeRelease();
    Super::EndPlay(EndPlayReason);
}

void UShaderProfilerTickingBridge::PushMatrixUpdateAsync(FVertexMemoryBlock* InDataBlock, UMaterialInstanceDynamic* InTargetMaterial)
{
    if (!InDataBlock || !InTargetMaterial || bQueryIssued) return;

    int32 ElementsToCopy = FMath::Min(InDataBlock->ActiveElementCount, MaxBufferElements);

    // Pack the data block matrices into continuous RGBA linear formats for the shader cache
    for (int32 i = 0; i < ElementsToCopy; ++i)
    {
        GPUVectorBuffer[i] = FLinearColor(InDataBlock->MatrixBuffer[i].X, InDataBlock->MatrixBuffer[i].Y, InDataBlock->MatrixBuffer[i].Z, 1.0f);
    }
    for (int32 i = ElementsToCopy; i < MaxBufferElements; ++i)
    {
        GPUVectorBuffer[i] = FLinearColor::Black;
    }

    // Direct Parameter Submission to the Dynamic Material register line
    InTargetMaterial->SetVectorParameterValue(TEXT("DeformationVectorArray"), GPUVectorBuffer);

    // Inject hardware profiling markers directly into the rendering pipeline queue
    ENQUEUE_RENDER_COMMAND(ProfileShaderExecutionTime)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            if (TimestampStartQuery.IsValid() && TimestampEndQuery.IsValid())
            {
                // Mark timestamp start immediately before the next graphics driver draw pass execution
                RHICmdList.EndRenderQuery(TimestampStartQuery);
                
                // [Implicit Driver Phase: Unreal Engine Vertex Shader Matrix Evaluation Sequence Executed Here]
                
                // Mark timestamp finish boundary point
                RHICmdList.EndRenderQuery(TimestampEndQuery);
                bQueryIssued = true;
            }
        });
}

void UShaderProfilerTickingBridge::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bQueryIssued)
    {
        uint64 StartTimeRaw = 0;
        uint64 EndTimeRaw = 0;

        // Query the hardware buffer data loops non-blockingly
        bool bStartReady = RHIGetRenderQueryResult(TimestampStartQuery, StartTimeRaw, false);
        bool bEndReady = RHIGetRenderQueryResult(TimestampEndQuery, EndTimeRaw, false);

        if (bStartReady && bEndReady)
        {
            // Convert raw internal GPU clock cycles into clear microsecond metrics
            uint64 DeltaCycles = EndTimeRaw - StartTimeRaw;
            float Microseconds = (float)DeltaCycles * (1000000.0f / (float)GHZFrequency); // Normalizes against GPU clock rates

            LatestTelemetry.ExecutionTimeMicroseconds = FMath::RoundToInt(Microseconds);
            LatestTelemetry.bTelemetryValid = true;
            bQueryIssued = false; // Release lock flag to allow the next matrix loop injection path

            UE_LOG(LogTemp, Log, TEXT("[+] GPU Pipeline Telemetry Logged: Deformation Vector Shader Execution Time = %d microseconds."), LatestTelemetry.ExecutionTimeMicroseconds);
        }
    }
}
