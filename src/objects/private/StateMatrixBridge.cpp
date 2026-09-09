#include "StateMatrixBridge.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UStateMatrixBridge::UStateMatrixBridge()
{
    PrimaryComponentTick.bCanEverTick = false; // Initial configuration utilizes event-driven ingestion updates
    HapticBaselineVoltageMV = 290.0f;
    HapticBaselineFrequencyHz = 100.0f;
    ZooidAdhesionCoefficient = 0.74f;
}

void UStateMatrixBridge::BeginPlay()
{
    Super::BeginPlay();
    
    // Automatically query for local directory configuration matrix updates
    FString DefaultMatrixPath = FPaths::ProjectContentDir() / TEXT("Data/state_matrix.json");
    IngestStateMatrixConfiguration(DefaultMatrixPath);
}

bool UStateMatrixBridge::IngestStateMatrixConfiguration(const FString& FilePath)
{
    FString JsonContentString;
    if (!FFileHelper::LoadFileToString(JsonContentString, *FilePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("[!] RT C++ Bridge Matrix Load Failure: Unable to locate source file path: %s"), *FilePath);
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonContentString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        // 1. Unpack Nested Tissue Yield Data Thresholds
        if (JsonObject->HasField(TEXT("tissue_yield_thresholds_mpa")))
        {
            TSharedPtr<FJsonObject> TissueObj = JsonObject->GetObjectField(TEXT("tissue_yield_thresholds_mpa"));
            ActiveYieldThresholds.BrainNeuralMatrixMPa = TissueObj->GetNumberField(TEXT("brain_neural_matrix"));
            ActiveYieldThresholds.MacroArteryTrunkMPa = TissueObj->GetNumberField(TEXT("macro_artery_trunk"));
            ActiveYieldThresholds.CapillaryBedBoundaryMPa = TissueObj->GetNumberField(TEXT("capillary_bed_boundary"));
        }

        // 2. Unpack Haptic Signal Constraints
        if (JsonObject->HasField(TEXT("haptic_baseline")))
        {
            TSharedPtr<FJsonObject> HapticObj = JsonObject->GetObjectField(TEXT("haptic_baseline"));
            HapticBaselineVoltageMV = HapticObj->GetNumberField(TEXT("voltage_mv"));
            HapticBaselineFrequencyHz = HapticObj->GetNumberField(TEXT("frequency_hz"));
        }

        // 3. Unpack Zooid Simulation Variables
        if (JsonObject->HasField(TEXT("zooid_transport")))
        {
            TSharedPtr<FJsonObject> ZooidObj = JsonObject->GetObjectField(TEXT("zooid_transport"));
            ZooidAdhesionCoefficient = ZooidObj->GetNumberField(TEXT("base_adhesion_coefficient"));
        }

        UE_LOG(LogTemp, Log, TEXT("[+] RT C++ Bridge Synchronization Matrix Successfully Connected: %s"), *FilePath);
        return true;
    }

    UE_LOG(LogTemp, Error, TEXT("[!] CRITICAL INTERFACE ERROR: JSON Structural Parsing Breakdown inside: %s"), *FilePath);
    return false;
}
