// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DwarfUndergroundBaseTile.h"
#include "DwarfUndergroundBuildManager.h"
#include "./MapBase/Soil.h"
#include "SoilManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DWARFUNDERGROUND_API ASoilManager : public AActor
{
    GENERATED_BODY()

public:
    ASoilManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Reference to the build manager
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
    ADwarfUndergroundBuildManager* BuildManager;

    // Add soil to multiple tiles in an area
    UFUNCTION(BlueprintCallable, Category = "Soil Management")
    void AddSoilToArea(const FVector& Center, float Radius, int32 NumLayers = 1);

    // Remove top soil from multiple tiles in an area
    UFUNCTION(BlueprintCallable, Category = "Soil Management")
    void RemoveSoilFromArea(const FVector& Center, float Radius, int32 NumLayers = 1);

    // Set the height of an area to a specific number of soil layers
    UFUNCTION(BlueprintCallable, Category = "Soil Management")
    void SetAreaSoilHeight(const FVector& Center, float Radius, int32 TargetLayers);

    // Create a randomized terrain using soil layers
    UFUNCTION(BlueprintCallable, Category = "Soil Management")
    void GenerateRandomTerrain(float MaxHeight = 5, float Smoothness = 0.5);

    // Create a soil pattern (e.g., hill, valley, plateau)
    // UFUNCTION(BlueprintCallable, Category = "Soil Management")
    // void CreateSoilPattern(EBuildingType PatternType, const FVector& Center, float Size);

    // Find a tile at a specific location
    UFUNCTION(BlueprintCallable, Category = "Tile Management")
    ADwarfUndergroundBaseTile* FindTileAt(const FVector& Location);

    // Get tiles within an area
    UFUNCTION(BlueprintCallable, Category = "Tile Management")
    TArray<ADwarfUndergroundBaseTile*> GetTilesInArea(const FVector& Center, float Radius);
};