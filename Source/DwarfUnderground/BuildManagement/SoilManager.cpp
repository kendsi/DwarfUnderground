// Fill out your copyright notice in the Description page of Project Settings.


#include "SoilManager.h"
#include "Kismet/GameplayStatics.h"

ASoilManager::ASoilManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ASoilManager::BeginPlay()
{
    Super::BeginPlay();

    if (!BuildManager)
    {
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADwarfUndergroundBuildManager::StaticClass(), FoundActors);

        if (FoundActors.Num() > 0)
        {
            BuildManager = Cast<ADwarfUndergroundBuildManager>(FoundActors[0]);
        }
    }
}

void ASoilManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

ADwarfUndergroundBaseTile* ASoilManager::FindTileAt(const FVector& Location)
{
    if (!BuildManager) return nullptr;

    ADwarfUndergroundBaseTile* ClosestTile = nullptr;
    float ClosestDistanceSq = FLT_MAX;

    for (ADwarfUndergroundBaseTile* Tile : BuildManager->AllSpawnedTiles)
    {
        if (Tile)
        {
            FVector TileXY = Tile->GetActorLocation();
            TileXY.Z = Location.Z;

            float DistanceSq = FVector::DistSquared(TileXY, Location);
            if (DistanceSq < ClosestDistanceSq)
            {
                ClosestDistanceSq = DistanceSq;
                ClosestTile = Tile;
            }
        }
    }

    return ClosestTile;
}

TArray<ADwarfUndergroundBaseTile*> ASoilManager::GetTilesInArea(const FVector& Center, float Radius)
{
    TArray<ADwarfUndergroundBaseTile*> TilesInArea;

    if (!BuildManager) return TilesInArea;

    float RadiusSquared = Radius * Radius;

    for (ADwarfUndergroundBaseTile* Tile : BuildManager->AllSpawnedTiles)
    {
        if (Tile)
        {
            FVector TileXY = Tile->GetActorLocation();
            TileXY.Z = Center.Z;

            if (FVector::DistSquared(TileXY, Center) <= RadiusSquared)
            {
                TilesInArea.Add(Tile);
            }
        }
    }

    return TilesInArea;
}

void ASoilManager::AddSoilToArea(const FVector& Center, float Radius, int32 NumLayers)
{
    if (!BuildManager) return;

    TArray<ADwarfUndergroundBaseTile*> TilesInArea = GetTilesInArea(Center, Radius);

    for (ADwarfUndergroundBaseTile* Tile : TilesInArea)
    {
        for (int32 i = 0; i < NumLayers; i++)
        {
            BuildManager->SpawnSoilOnTile(Tile);
        }
    }
}

void ASoilManager::RemoveSoilFromArea(const FVector& Center, float Radius, int32 NumLayers)
{
    if (!BuildManager) return;

    TArray<ADwarfUndergroundBaseTile*> TilesInArea = GetTilesInArea(Center, Radius);

    for (ADwarfUndergroundBaseTile* Tile : TilesInArea)
    {
        for (int32 i = 0; i < NumLayers; i++)
        {
            BuildManager->RemoveTopSoilFromTile(Tile);
        }
    }
}

void ASoilManager::SetAreaSoilHeight(const FVector& Center, float Radius, int32 TargetLayers)
{
    if (!BuildManager) return;

    TArray<ADwarfUndergroundBaseTile*> TilesInArea = GetTilesInArea(Center, Radius);

    for (ADwarfUndergroundBaseTile* Tile : TilesInArea)
    {
        int32 CurrentLayers = Tile->SoilStack.Num();

        if (CurrentLayers < TargetLayers)
        {
            for (int32 i = 0; i < (TargetLayers - CurrentLayers); i++)
            {
                BuildManager->SpawnSoilOnTile(Tile);
            }
        }
        else if (CurrentLayers > TargetLayers)
        {
            for (int32 i = 0; i < (CurrentLayers - TargetLayers); i++)
            {
                BuildManager->RemoveTopSoilFromTile(Tile);
            }
        }
    }
}

void ASoilManager::GenerateRandomTerrain(float MaxHeight, float Smoothness)
{
    if (!BuildManager) return;

    TArray<ADwarfUndergroundBaseTile*>& AllTiles = BuildManager->AllSpawnedTiles;

    for (ADwarfUndergroundBaseTile* Tile : AllTiles)
    {
        while (Tile->SoilStack.Num() > 0)
        {
            BuildManager->RemoveTopSoilFromTile(Tile);
        }
    }

    for (ADwarfUndergroundBaseTile* Tile : AllTiles)
    {
        FVector TilePos = Tile->GetActorLocation();

        float Seed = (FMath::Sin(TilePos.X * 0.01 * Smoothness) +
            FMath::Cos(TilePos.Y * 0.01 * Smoothness)) * 0.5 + 0.5;

        int32 NumLayers = FMath::RoundToInt(Seed * MaxHeight);

        for (int32 i = 0; i < NumLayers; i++)
        {
            BuildManager->SpawnSoilOnTile(Tile);
        }
    }
}

// void ASoilManager::CreateSoilPattern(EBuildingType PatternType, const FVector& Center, float Size)
// {
//     if (!BuildManager) return;
//
//     switch (PatternType)
//     {
//     case EBuildingType::HouseLv1:
//     {
//         for (ADwarfUndergroundBaseTile* Tile : BuildManager->AllSpawnedTiles)
//         {
//             FVector TilePos = Tile->GetActorLocation();
//             TilePos.Z = Center.Z;
//
//             float Distance = FVector::Dist(TilePos, Center);
//             if (Distance <= Size)
//             {
//                 float HeightRatio = 1.0f - (Distance / Size);
//                 int32 NumLayers = FMath::RoundToInt(HeightRatio * 5);
//
//                 while (Tile->SoilStack.Num() > 0)
//                 {
//                     BuildManager->RemoveTopSoilFromTile(Tile);
//                 }
//
//                 for (int32 i = 0; i < NumLayers; i++)
//                 {
//                     BuildManager->SpawnSoilOnTile(Tile);
//                 }
//             }
//         }
//         break;
//     }
//
//     case EBuildingType::RefineryLv1:
//     {
//         for (ADwarfUndergroundBaseTile* Tile : BuildManager->AllSpawnedTiles)
//         {
//             FVector TilePos = Tile->GetActorLocation();
//             TilePos.Z = Center.Z;
//
//             float Distance = FVector::Dist(TilePos, Center);
//             if (Distance <= Size)
//             {
//                 float HeightRatio = Distance / Size;
//                 int32 NumLayers = FMath::RoundToInt(HeightRatio * 5);
//
//                 while (Tile->SoilStack.Num() > 0)
//                 {
//                     BuildManager->RemoveTopSoilFromTile(Tile);
//                 }
//
//                 for (int32 i = 0; i < NumLayers; i++)
//                 {
//                     BuildManager->SpawnSoilOnTile(Tile);
//                 }
//             }
//         }
//         break;
//     }
//
//     default:
//     {
//         for (ADwarfUndergroundBaseTile* Tile : BuildManager->AllSpawnedTiles)
//         {
//             FVector TilePos = Tile->GetActorLocation();
//             TilePos.Z = Center.Z;
//
//             float Distance = FVector::Dist(TilePos, Center);
//             if (Distance <= Size)
//             {
//                 while (Tile->SoilStack.Num() > 0)
//                 {
//                     BuildManager->RemoveTopSoilFromTile(Tile);
//                 }
//
//                 for (int32 i = 0; i < 3; i++)
//                 {
//                     BuildManager->SpawnSoilOnTile(Tile);
//                 }
//             }
//         }
//         break;
//     }
//     }
// }