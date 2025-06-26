#include "DwarfUndergroundBuildManager.h"
#include "DwarfUndergroundBaseTile.h"
#include "NavigationSystem.h"
#include "DwarfUnderground/Item/InventorySubSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// 생성자
ADwarfUndergroundBuildManager::ADwarfUndergroundBuildManager()
{
    PrimaryActorTick.bCanEverTick = false;
    LevelTilesArray.Empty();
    AllSpawnedTiles.Empty();
    CurrentLevelIndex = 0;
}

void ADwarfUndergroundBuildManager::BeginPlay()
{
    Super::BeginPlay();

    AddNewLevel(TEXT("1층"), -910.0f);
    bCreateSoilWithTile = true;
    InitialSoilLayers = 2;

    SoilLayerThickness = 75.f;  // 위치 간격은 75
    // 하지만 실제 흙 크기는 60으로 고정

    SetCurrentLevel(0);
    SpawnRectangularTilePattern(100, 70, FVector(7700, 1800, -910));
}

void ADwarfUndergroundBuildManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// === 층 관리 함수들 ===

int32 ADwarfUndergroundBuildManager::AddNewLevel(const FString& LevelName, float LevelZ)
{
    FLevelTiles NewLevel;
    NewLevel.LevelName = LevelName;
    NewLevel.LevelZ = LevelZ;

    int32 NewIndex = LevelTilesArray.Add(NewLevel);
    UE_LOG(LogTemp, Display, TEXT("새 층이 추가되었습니다: %s (Z: %.1f, 인덱스: %d)"),
        *LevelName, LevelZ, NewIndex);

    return NewIndex;
}

bool ADwarfUndergroundBuildManager::RemoveLevel(int32 LevelIndex)
{
    if (!IsValidLevelIndex(LevelIndex))
        return false;

    ClearLevelTiles(LevelIndex);
    LevelTilesArray.RemoveAt(LevelIndex);

    // 현재 층 인덱스 조정
    if (CurrentLevelIndex >= LevelTilesArray.Num())
        CurrentLevelIndex = FMath::Max(0, LevelTilesArray.Num() - 1);

    SyncAllTilesFromLevels();
    return true;
}

void ADwarfUndergroundBuildManager::ClearLevelTiles(int32 LevelIndex)
{
    if (!IsValidLevelIndex(LevelIndex))
        return;

    for (ADwarfUndergroundBaseTile* Tile : LevelTilesArray[LevelIndex].Tiles)
    {
        if (Tile)
            Tile->Destroy();
    }

    LevelTilesArray[LevelIndex].Tiles.Empty();
    SyncAllTilesFromLevels();
}

void ADwarfUndergroundBuildManager::SetCurrentLevel(int32 LevelIndex)
{
    if (!IsValidLevelIndex(LevelIndex))
        return;

    CurrentLevelIndex = LevelIndex;
    UE_LOG(LogTemp, Display, TEXT("현재 층이 변경되었습니다: %s"), *LevelTilesArray[CurrentLevelIndex].LevelName);
}

void ADwarfUndergroundBuildManager::SyncAllTilesFromLevels()
{
    AllSpawnedTiles.Empty();
    for (const FLevelTiles& Level : LevelTilesArray)
        AllSpawnedTiles.Append(Level.Tiles);
}

// === 타일 생성 함수들 ===

TArray<FVector2D> ADwarfUndergroundBuildManager::GenerateRectangularPattern(int32 Width, int32 Height)
{
    TArray<FVector2D> Pattern;
    const int32 HalfWidth = Width / 2;
    const int32 HalfHeight = Height / 2;

    for (int32 y = -HalfHeight; y < Height - HalfHeight; y++)
    {
        for (int32 x = -HalfWidth; x < Width - HalfWidth; x++)
        {
            Pattern.Add(FVector2D(x, y));
        }
    }

    return Pattern;
}

void ADwarfUndergroundBuildManager::SpawnRectangularTilePattern(int32 Width, int32 Height, const FVector& Center)
{
    SpawnRectangularTilePatternAtLevel(CurrentLevelIndex, Width, Height, FVector2D(Center.X, Center.Y));
}

void ADwarfUndergroundBuildManager::SpawnRectangularTilePatternAtLevel(int32 LevelIndex, int32 Width, int32 Height, const FVector2D& CenterXY)
{
    if (!IsValidLevelIndex(LevelIndex))
        return;

    // 기존 층의 타일 제거
    ClearLevelTiles(LevelIndex);

    // 타일 클래스 확인
    TSubclassOf<ADwarfUndergroundBaseTile> ClassToUse = GetTileClassToUse();

    const float TileWidth = TILE_WIDTH;
    const float TileHeight = TILE_HEIGHT;
    float LevelZ = LevelTilesArray[LevelIndex].LevelZ;

    TArray<FVector2D> Pattern = GenerateRectangularPattern(Width, Height);

    if (UWorld* World = GetWorld())
    {
        for (const FVector2D& Coord : Pattern)
        {
            float xPos = CenterXY.X + Coord.X * TileWidth;
            float yPos = CenterXY.Y + Coord.Y * TileHeight;

            FVector Location(xPos, yPos, LevelZ);

            SpawnTileAtLocation(World, ClassToUse, Location,
                FMath::RoundToInt(Coord.X), FMath::RoundToInt(Coord.Y), LevelIndex);
        }
    }

    SyncAllTilesFromLevels();

    UE_LOG(LogTemp, Display, TEXT("%s 층에 사각형 패턴으로 타일 %d개를 생성했습니다."),
        *LevelTilesArray[LevelIndex].LevelName, LevelTilesArray[LevelIndex].Tiles.Num());
}

void ADwarfUndergroundBuildManager::SpawnTileAtLocation(UWorld* World, TSubclassOf<ADwarfUndergroundBaseTile> ClassToUse,
    const FVector& Location, int32 GridX, int32 GridY, int32 LevelIndex)
{
    if (!World || !IsValidLevelIndex(LevelIndex))
        return;

    if (!HasFloorBelow(Location))
    {
        UE_LOG(LogTemp, Verbose,
            TEXT("Floor not found under (%d,%d) – tile skipped"), GridX, GridY);
        return;
    }

    if (IsBlockedByOtherActors(Location))
    {
        UE_LOG(LogTemp, Verbose,
            TEXT("Blocked by actor at (%d,%d) – tile skipped"), GridX, GridY);
        return;
    }

    FRotator Rotation = FRotator::ZeroRotator;
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;

    ADwarfUndergroundBaseTile* SpawnedTile = World->SpawnActor<ADwarfUndergroundBaseTile>(
        ClassToUse,
        Location,
        Rotation,
        SpawnParams
    );

    if (SpawnedTile)
    {
        SpawnedTile->SetActorScale3D(FVector(1.2f, 1.6f, 1.0f));
        SpawnedTile->GridX = GridX;
        SpawnedTile->GridY = GridY;
        SpawnedTile->BaseStoneZ = LevelTilesArray[LevelIndex].LevelZ;

        LevelTilesArray[LevelIndex].Tiles.Add(SpawnedTile);

        if (bCreateSoilWithTile)
        {
            for (int32 i = 0; i < InitialSoilLayers; i++)
            {
                SpawnSoilOnTile(SpawnedTile);
            }
            AdjustTilePositionByLevel(SpawnedTile);
        }
    }
}

ADwarfUndergroundBaseTile* ADwarfUndergroundBuildManager::GetTileAtLevel(int32 LevelIndex, int32 GridX, int32 GridY)
{
    if (!IsValidLevelIndex(LevelIndex))
        return nullptr;

    for (ADwarfUndergroundBaseTile* Tile : LevelTilesArray[LevelIndex].Tiles)
    {
        if (Tile && Tile->GridX == GridX && Tile->GridY == GridY)
        {
            return Tile;
        }
    }

    return nullptr;
}

ADwarfUndergroundBaseTile* ADwarfUndergroundBuildManager::GetTileAt(int32 GridX, int32 GridY)
{
    ADwarfUndergroundBaseTile* Tile = GetTileAtLevel(CurrentLevelIndex, GridX, GridY);

    if (!Tile)
    {
        for (ADwarfUndergroundBaseTile* AnyTile : AllSpawnedTiles)
        {
            if (AnyTile && AnyTile->GridX == GridX && AnyTile->GridY == GridY)
            {
                return AnyTile;
            }
        }
    }

    return Tile;
}

// === 흙 관련 함수들 ===

ASoil* ADwarfUndergroundBuildManager::CreateSoilAtLocation(const FVector& Location, ESoilType SoilType, float Height)
{
    if (!GetWorld()) return nullptr;

    FActorSpawnParameters P;
    P.Owner = this;
    ASoil* Soil = GetWorld()->SpawnActor<ASoil>(
        ASoil::StaticClass(), Location, FRotator::ZeroRotator, P);

    if (!Soil)
        return nullptr;

    Soil->SetSoilType(ESoilType::Normal);
    Soil->SoilHeight = 60.f;

    const float ScaleX = TILE_WIDTH * 1.175 / SoilMeshBaseSize;
    const float ScaleY = TILE_HEIGHT * 1.175 / SoilMeshBaseSize;
    const float ScaleZ = 60.f * 2.5 / SoilMeshBaseSize;
    Soil->SetActorScale3D(FVector(ScaleX, ScaleY, ScaleZ));

    UE_LOG(LogTemp, Log, TEXT("흙 생성됨: 위치=(%s), 타입=Normal"), *Location.ToString());

    return Soil;
}

void ADwarfUndergroundBuildManager::SpawnSoilOnTile(ADwarfUndergroundBaseTile* Tile)
{
    if (!Tile || !HasFloorBelow(Tile->GetActorLocation()))
        return;

    if (IsBlockedByOtherActors(Tile->GetActorLocation()))
        return;

    float LevelBaseZ = 0.0f;
    for (int32 i = 0; i < LevelTilesArray.Num(); i++)
    {
        if (LevelTilesArray[i].Tiles.Contains(Tile))
        {
            LevelBaseZ = LevelTilesArray[i].LevelZ;
            break;
        }
    }

    FVector SoilLocation = Tile->GetActorLocation();
    const int32 CurrentSoilCount = Tile->SoilStack.Num();
    SoilLocation.Z = LevelBaseZ + (CurrentSoilCount * SoilLayerThickness);

    if (ASoil* NewSoil = CreateSoilAtLocation(SoilLocation, ESoilType::Normal))
    {
        NewSoil->AddOccupiedTile(Tile);
        Tile->SoilStack.Add(NewSoil);
        UpdateTilePositionAboveSoil(Tile);
        UpdateNavMeshAroundLocation(SoilLocation);

        UE_LOG(LogTemp, Display, TEXT("흙 설치됨: 타일 (%d,%d), 현재 스택 크기: %d"),
            Tile->GridX, Tile->GridY, Tile->SoilStack.Num());
    }
}

void ADwarfUndergroundBuildManager::SpawnMultipleSoilLayersOnTile(ADwarfUndergroundBaseTile* TargetTile, int32 NumLayers, ESoilType SoilType)
{
    if (!TargetTile || NumLayers <= 0)
        return;

    for (int32 i = 0; i < NumLayers; i++)
    {
        SpawnSoilOnTile(TargetTile);
    }
}

void ADwarfUndergroundBuildManager::AddSoilToLevelTiles(int32 LevelIndex, int32 NumLayers, ESoilType SoilType)
{
    if (!IsValidLevelIndex(LevelIndex) || NumLayers <= 0)
        return;

    for (ADwarfUndergroundBaseTile* Tile : LevelTilesArray[LevelIndex].Tiles)
    {
        if (Tile)
        {
            SpawnMultipleSoilLayersOnTile(Tile, NumLayers, ESoilType::Normal);
        }
    }

    UE_LOG(LogTemp, Display, TEXT("%s 층의 타일에 흙 %d층을 추가했습니다."),
        *LevelTilesArray[LevelIndex].LevelName, NumLayers);
}

void ADwarfUndergroundBuildManager::RemoveTopSoilFromTile(ADwarfUndergroundBaseTile* TargetTile)
{
    if (!TargetTile || HasBuildingOnTile(TargetTile)) return;

    TArray<ASoil*>& Soils = TargetTile->SoilStack;
    if (Soils.Num() > 0)
    {
        FVector SoilLocation = Soils.Last()->GetActorLocation();

        ASoil* TopSoil = Soils.Last();
        if (TopSoil)
        {
            TopSoil->Destroy();
        }
        Soils.RemoveAt(Soils.Num() - 1);

        UpdateTilePositionAboveSoil(TargetTile);
        UpdateNavMeshAroundLocation(SoilLocation);

        //06.25: 흙 +1
        if (auto* Inven = GetGameInstance()->GetSubsystem<UInventorySubSystem>())
        {
            Inven->AddItem(FName("Dirt"), 1);            
        }
    }
}

void ADwarfUndergroundBuildManager::RemoveMultipleSoilLayersFromTile(ADwarfUndergroundBaseTile* TargetTile, int32 NumLayers)
{
    if (!TargetTile || NumLayers <= 0 || HasBuildingOnTile(TargetTile)) return;

    for (int32 i = 0; i < NumLayers; i++)
    {
        if (TargetTile->SoilStack.Num() <= 0)
            break;

        RemoveTopSoilFromTile(TargetTile);
    }
}

void ADwarfUndergroundBuildManager::ClearAllSoilFromTile(ADwarfUndergroundBaseTile* TargetTile)
{
    if (!TargetTile || HasBuildingOnTile(TargetTile)) return;

    int32 TileLevelIndex = -1;
    float LevelBaseZ = 0.0f;

    for (int32 i = 0; i < LevelTilesArray.Num(); i++)
    {
        if (LevelTilesArray[i].Tiles.Contains(TargetTile))
        {
            TileLevelIndex = i;
            LevelBaseZ = LevelTilesArray[i].LevelZ;
            break;
        }
    }

    while (TargetTile->SoilStack.Num() > 0)
    {
        ASoil* TopSoil = TargetTile->SoilStack.Last();
        TargetTile->SoilStack.RemoveAt(TargetTile->SoilStack.Num() - 1);
        if (TopSoil)
        {
            TopSoil->Destroy();
        }
    }

    if (TileLevelIndex != -1)
    {
        FVector NewLocation = TargetTile->GetActorLocation();
        NewLocation.Z = LevelBaseZ;
        TargetTile->SetActorLocation(NewLocation);
    }
}

void ADwarfUndergroundBuildManager::UpdateTilePositionAboveSoil(ADwarfUndergroundBaseTile* Tile)
{
    if (!Tile)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateTilePositionAboveSoil: Tile이 nullptr입니다."));
        return;
    }

    float LevelBaseZ = 0.0f;
    bool bFoundLevel = false;

    for (int32 i = 0; i < LevelTilesArray.Num(); i++)
    {
        if (LevelTilesArray[i].Tiles.Contains(Tile))
        {
            LevelBaseZ = LevelTilesArray[i].LevelZ;
            bFoundLevel = true;
            break;
        }
    }

    if (!bFoundLevel)
    {
        LevelBaseZ = Tile->BaseStoneZ;
    }

    FVector CurrentLocation = Tile->GetActorLocation();
    FVector NewTileLocation = CurrentLocation;

    if (Tile->SoilStack.Num() == 0)
    {
        NewTileLocation.Z = LevelBaseZ;
    }
    else
    {
        float CalculatedZ = LevelBaseZ + (Tile->SoilStack.Num() * SoilLayerThickness) + 10.0f;
        NewTileLocation.Z = CalculatedZ;
    }

    float HeightDifference = FMath::Abs(NewTileLocation.Z - CurrentLocation.Z);
    if (HeightDifference > 1.0f)
    {
        Tile->SetActorLocation(NewTileLocation);
    }

    if (Tile->PlacedBuilding)
    {
        AActor* PlacedBuildingActor = Tile->PlacedBuilding.GetInterface()->GetActor();
        if (PlacedBuildingActor)
        {
            FVector BuildLoc = PlacedBuildingActor->GetActorLocation();
            BuildLoc.Z = NewTileLocation.Z - 5.f;
            PlacedBuildingActor->SetActorLocation(BuildLoc);
        }
    }
}

void ADwarfUndergroundBuildManager::AdjustTilePositionByLevel(ADwarfUndergroundBaseTile* Tile)
{
    if (!Tile)
        return;

    int32 TileLevelIndex = -1;
    float LevelBaseZ = 0.0f;

    for (int32 i = 0; i < LevelTilesArray.Num(); i++)
    {
        if (LevelTilesArray[i].Tiles.Contains(Tile))
        {
            TileLevelIndex = i;
            LevelBaseZ = LevelTilesArray[i].LevelZ;
            break;
        }
    }

    if (TileLevelIndex == -1)
    {
        return;
    }

    for (int32 i = 0; i < Tile->SoilStack.Num(); i++)
    {
        ASoil* Soil = Tile->SoilStack[i];
        if (Soil)
        {
            FVector SoilPos = Soil->GetActorLocation();
            SoilPos.Z = LevelBaseZ + (i * SoilLayerThickness);
            Soil->SetActorLocation(SoilPos);
        }
    }

    UpdateTilePositionAboveSoil(Tile);
}

void ADwarfUndergroundBuildManager::SetAllTilesOpacity(float InOpacity)
{
    for (ADwarfUndergroundBaseTile* Tile : AllSpawnedTiles)
    {
        if (Tile)
        {
            Tile->SetTileOpacity(InOpacity);
        }
    }
}

// === 건물 배치 관련 함수들 ===
TArray<ADwarfUndergroundBaseTile*> ADwarfUndergroundBuildManager::GetAdjacentTiles(ADwarfUndergroundBaseTile* OriginTile, int32 Width, int32 Height)
{
    TArray<ADwarfUndergroundBaseTile*> Tiles;

    if (!OriginTile || Width <= 0 || Height <= 0)
        return Tiles;

    int32 StartX = OriginTile->GridX;
    int32 StartY = OriginTile->GridY;

    for (int32 y = 0; y < Height; y++)
    {
        for (int32 x = 0; x < Width; x++)
        {
            ADwarfUndergroundBaseTile* Tile = GetTileAt(StartX + x, StartY + y);
            if (!Tile)
                return TArray<ADwarfUndergroundBaseTile*>();
        }
    }

    Tiles.Add(OriginTile);

    for (int32 y = 0; y < Height; y++)
    {
        for (int32 x = 0; x < Width; x++)
        {
            if (x == 0 && y == 0)
                continue;

            ADwarfUndergroundBaseTile* Tile = GetTileAt(StartX + x, StartY + y);
            Tiles.Add(Tile);
        }
    }

    return Tiles;
}

bool ADwarfUndergroundBuildManager::CanPlaceBuilding(ADwarfUndergroundBaseTile* OriginTile, int32 Width, int32 Height)
{
    if (!OriginTile)
        return false;

    TArray<ADwarfUndergroundBaseTile*> RequiredTiles = GetAdjacentTiles(OriginTile, Width, Height);

    if (RequiredTiles.Num() != Width * Height)
        return false;

    for (ADwarfUndergroundBaseTile* Tile : RequiredTiles)
    {
        if (Tile->bIsOccupied)
            return false;
    }

    return true;
}

void ADwarfUndergroundBuildManager::UpdateNavMeshAroundLocation(const FVector& Location)
{
    if (UWorld* World = GetWorld())
    {
        if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World))
        {
            FVector BoxExtent(TILE_WIDTH * 2.f, TILE_HEIGHT * 2.f, SoilLayerThickness * 4.f);
            FBox UpdateBounds = FBox::BuildAABB(Location, BoxExtent);
            NavSys->AddDirtyArea(UpdateBounds, ENavigationDirtyFlag::All);
            UE_LOG(LogTemp, Log, TEXT("NavMesh 더티 영역 추가: %s"), *Location.ToString());
        }
    }
}

void ADwarfUndergroundBuildManager::ForceUpdateAllTilePositions()
{
    UE_LOG(LogTemp, Warning, TEXT("모든 타일 위치 강제 업데이트 시작"));

    int32 UpdatedCount = 0;
    for (ADwarfUndergroundBaseTile* Tile : AllSpawnedTiles)
    {
        if (Tile)
        {
            UpdateTilePositionAboveSoil(Tile);
            UpdatedCount++;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("총 %d개 타일 업데이트 완료"), UpdatedCount);
}

// === Private 함수들 ===

bool ADwarfUndergroundBuildManager::IsValidLevelIndex(int32 LevelIndex)
{
    if (LevelIndex < 0 || LevelIndex >= LevelTilesArray.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("유효하지 않은 층 인덱스입니다: %d"), LevelIndex);
        return false;
    }
    return true;
}

TSubclassOf<ADwarfUndergroundBaseTile> ADwarfUndergroundBuildManager::GetTileClassToUse()
{
    TSubclassOf<ADwarfUndergroundBaseTile> ClassToReturn = TileClass;
    if (!ClassToReturn)
    {
        ClassToReturn = ADwarfUndergroundBaseTile::StaticClass();
    }
    return ClassToReturn;
}

bool ADwarfUndergroundBuildManager::HasFloorBelow(const FVector& Location) const
{
    const FVector Start = Location;
    const FVector End = Location - FVector(0.f, 0.f, 30.f);

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(const_cast<ADwarfUndergroundBuildManager*>(this));

    return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
}

bool ADwarfUndergroundBuildManager::IsBlockedByOtherActors(const FVector& Location, const FVector& HalfExtent) const
{
    if (!GetWorld()) return true;

    TArray<FHitResult> Hits;
    FCollisionQueryParams Params;
    Params.bTraceComplex = false;
    Params.AddIgnoredActor(const_cast<ADwarfUndergroundBuildManager*>(this));

    // 디버그: 어떤 채널을 사용하고 있는지 확인
    UE_LOG(LogTemp, Warning, TEXT("IsBlockedByOtherActors 체크 시작: 위치 %s"), *Location.ToString());

    const bool bHit = GetWorld()->SweepMultiByChannel(
        Hits,
        Location, Location,
        FQuat::Identity,
        ECC_WorldStatic,  // 이 채널이 roadManager와 일치하는지 확인
        FCollisionShape::MakeBox(HalfExtent),
        Params);

    UE_LOG(LogTemp, Warning, TEXT("Sweep 결과: %s, Hit 개수: %d"), bHit ? TEXT("충돌됨") : TEXT("충돌안됨"), Hits.Num());

    if (!bHit) return false;

    for (const FHitResult& H : Hits)
    {
        if (!H.GetActor()) continue;

        UE_LOG(LogTemp, Warning, TEXT("충돌된 액터: %s (클래스: %s)"),
            *H.GetActor()->GetName(),
            *H.GetActor()->GetClass()->GetName());

        // 타일과 흙은 무시
        if (H.GetActor()->IsA<ADwarfUndergroundBaseTile>() ||
            H.GetActor()->IsA<ASoil>())
        {
            UE_LOG(LogTemp, Log, TEXT("무시됨: %s (타일 또는 흙)"), *H.GetActor()->GetName());
            continue;
        }

        // roadManager 체크 추가
        if (H.GetActor()->IsA<AroadManager>())
        {
            UE_LOG(LogTemp, Warning, TEXT("★ roadManager 발견! 건설 차단: %s"), *H.GetActor()->GetName());
            return true;
        }

        // 다른 액터가 있으면 블록됨
        UE_LOG(LogTemp, Warning, TEXT("건설이 블록됨: %s"), *H.GetActor()->GetName());
        return true;
    }

    UE_LOG(LogTemp, Log, TEXT("건설 허용: 블로킹 액터 없음"));
    return false;
}

bool ADwarfUndergroundBuildManager::HasBuildingOnTile(ADwarfUndergroundBaseTile* Tile) const
{
    if (!Tile) return false;

    if (Tile->PlacedBuilding || Tile->OccupyingBuilding)
    {
        UE_LOG(LogTemp, Warning, TEXT("건물이 있는 타일에서는 흙을 제거할 수 없습니다."));
        return true;
    }

    return false;
}