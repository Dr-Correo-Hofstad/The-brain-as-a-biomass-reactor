// ====================================================================
// DESIGN STANDARDS: RT 3D Visual Rendering System Substep Bridge
// Target Component: Unreal Engine 3D Substep Managers Mapping state_matrix.json
// ====================================================================

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "StateMatrixBridge.generated.h"

/**
 * Native C++ Struct defining the localized structural tissue yield limits.
 */
USTRUCT(BlueprintType)
struct FTissueYieldThresholds
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RT Bio-Metrics | Tissue Dynamics")
    float BrainNeuralMatrixMPa;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RT Bio-Metrics | Tissue Dynamics")
    float MacroArteryTrunkMPa;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RT Bio-Metrics | Tissue Dynamics")
    float CapillaryBedBoundaryMPa;

    FTissueYieldThresholds()
        : BrainNeuralMatrixMPa(0.03f)
        , MacroArteryTrunkMPa(2.80f)
        , CapillaryBedBoundaryMPa(0.12f)
    {}
};

/**
 * Central C++ Bridge Component for mapping JSON simulation outputs to Unreal Engine actuators.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THEBRAINASABIOMASSREACTOR_API UStateMatrixBridge : public UActorComponent
{
    GENERATED_BODY()

public:	
    UStateMatrixBridge();

protected:
    virtual void BeginPlay() override;

public:	
    /** Loads and parses the compiled state_matrix.json parameter array */
    UFUNCTION(BlueprintCallable, Category = "RT Data Ingestion | JSON")
    bool IngestStateMatrixConfiguration(const FString& FilePath);

    /** Current registered tissue threshold configuration maps */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RT Structural Config | State Matrix")
    FTissueYieldThresholds ActiveYieldThresholds;

    /** Base Haptic Voltage Metric (mV) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RT Structural Config | Haptic Baseline")
    float HapticBaselineVoltageMV;

    /** Base Haptic Frequency Metric (Hz) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RT Structural Config | Haptic Baseline")
    float HapticBaselineFrequencyHz;

    /** Zooid Target Transport Adhesion Coefficient */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RT Structural Config | Zooid Logistics")
    float ZooidAdhesionCoefficient;
};
