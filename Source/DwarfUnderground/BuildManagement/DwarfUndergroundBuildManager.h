#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DwarfUndergroundBaseTile.h"
#include "./MapBase/roadManager.h" 
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Engine/Engine.h"
#include "./MapBase/Soil.h"
#include "DwarfUndergroundBuildManager.generated.h"

// 각 층별 타일 정보를 저장하는 구조체
USTRUCT(BlueprintType)
struct FLevelTiles
{
    GENERATED_BODY()

    // 층 이름 (예: "1층", "지하1층" 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString LevelName;

    // 층의 Z 좌표
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LevelZ;

    // 이 층에 있는 모든 타일
    UPROPERTY(BlueprintReadOnly)
    TArray<ADwarfUndergroundBaseTile*> Tiles;

    FLevelTiles()
    {
        LevelName = TEXT("New Level");
        LevelZ = 0.0f;
        Tiles.Empty();
    }
};

UCLASS()
class DWARFUNDERGROUND_API ADwarfUndergroundBuildManager : public AActor
{
    GENERATED_BODY()

public:
    // 생성자 및 기본 함수
    ADwarfUndergroundBuildManager();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 타일 크기 상수
    static constexpr float TILE_WIDTH = 200.f;
    static constexpr float TILE_HEIGHT = 200.f;

    // === 층 관리 변수 및 함수들 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Management")
    TArray<FLevelTiles> LevelTilesArray;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Management")
    int32 CurrentLevelIndex;

    UPROPERTY(BlueprintReadOnly)
    TArray<ADwarfUndergroundBaseTile*> AllSpawnedTiles;

    // 층 추가 함수
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    int32 AddNewLevel(const FString& LevelName, float LevelZ);

    // 층 제거 함수
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    bool RemoveLevel(int32 LevelIndex);

    // 층의 모든 타일 제거
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void ClearLevelTiles(int32 LevelIndex);

    // 모든 층의 타일을 하나의 배열로 동기화
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void SyncAllTilesFromLevels();

    // 현재 작업 층 설정
    UFUNCTION(BlueprintCallable, Category = "Level Management")
    void SetCurrentLevel(int32 LevelIndex);

    // === 타일 생성 및 관리 함수들 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Layout")
    TSubclassOf<ADwarfUndergroundBaseTile> TileClass;

    // 사각형 패턴으로 타일 생성
    UFUNCTION(BlueprintCallable, Category = "Tile Layout")
    void SpawnRectangularTilePattern(int32 Width, int32 Height, const FVector& Center);

    // 지정된 층에 사각형 패턴으로 타일 생성
    UFUNCTION(BlueprintCallable, Category = "Tile Layout")
    void SpawnRectangularTilePatternAtLevel(int32 LevelIndex, int32 Width, int32 Height, const FVector2D& CenterXY);

    // 타일 생성
    UFUNCTION(BlueprintCallable, Category = "Tile Management")
    void SpawnTileAtLocation(UWorld* World, TSubclassOf<ADwarfUndergroundBaseTile> ClassToUse,
        const FVector& Location, int32 GridX, int32 GridY, int32 LevelIndex);

    // 타일 조회 함수
    UFUNCTION(BlueprintCallable, Category = "Tile Management")
    ADwarfUndergroundBaseTile* GetTileAtLevel(int32 LevelIndex, int32 GridX, int32 GridY);

    UFUNCTION(BlueprintCallable, Category = "Tile Management")
    ADwarfUndergroundBaseTile* GetTileAt(int32 GridX, int32 GridY);

    // === 흙 관련 함수들 ===
    // 타일 생성 시 함께 흙 생성 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soil")
    bool bCreateSoilWithTile = true;

    // 타일 생성 시 생성할 흙 층 개수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soil")
    int32 InitialSoilLayers = 1;

    // 위치에 흙 생성
    UFUNCTION(BlueprintCallable, Category = "Soil")
    ASoil* CreateSoilAtLocation(const FVector& Location, ESoilType SoilType = ESoilType::Normal, float Height = 5.0f);

    // 타일에 흙 추가
    UFUNCTION(BlueprintCallable, Category = "Soil")
    void SpawnSoilOnTile(ADwarfUndergroundBaseTile* TargetTile);

    UPROPERTY(EditAnywhere, Category = "Soil|Visual")
    float SoilLayerThickness = 60.f;        // 한 층 두께 (uu)

    UPROPERTY(EditAnywhere, Category = "Soil|Visual")
    float SoilMeshBaseSize = 200.f;        // 사용하는 정육면체 메시 한 변 길이

    // 타일에 여러 층의 흙 추가
    UFUNCTION(BlueprintCallable, Category = "Soil")
    void SpawnMultipleSoilLayersOnTile(ADwarfUndergroundBaseTile* TargetTile, int32 NumLayers, ESoilType SoilType = ESoilType::Normal);

    // 층의 모든 타일에 흙 추가
    UFUNCTION(BlueprintCallable, Category = "Soil")
    void AddSoilToLevelTiles(int32 LevelIndex, int32 NumLayers = 1, ESoilType SoilType = ESoilType::Normal);

    // 타일의 맨 위 흙 제거
    UFUNCTION(BlueprintCallable, Category = "Soil")
    void RemoveTopSoilFromTile(ADwarfUndergroundBaseTile* TargetTile);

    // 타일의 여러 층 흙 제거
    UFUNCTION(BlueprintCallable, Category = "Soil")
    void RemoveMultipleSoilLayersFromTile(ADwarfUndergroundBaseTile* TargetTile, int32 NumLayers);

    // 타일의 모든 흙 제거
    UFUNCTION(BlueprintCallable, Category = "Soil")
    void ClearAllSoilFromTile(ADwarfUndergroundBaseTile* TargetTile);

    // 타일 위치 업데이트 함수
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void UpdateTilePositionAboveSoil(ADwarfUndergroundBaseTile* Tile);

    // 타일 위치를 층별로 조정
    UFUNCTION(BlueprintCallable, Category = "Tile")
    void AdjustTilePositionByLevel(ADwarfUndergroundBaseTile* Tile);

    UFUNCTION(BlueprintCallable, Category = "Tile")
    void SetAllTilesOpacity(float InOpacity);

    UFUNCTION(BlueprintCallable, Category = "Building")
    TArray<ADwarfUndergroundBaseTile*> GetAdjacentTiles(
        ADwarfUndergroundBaseTile* OriginTile,
        int32 Width,
        int32 Height);

    UFUNCTION(BlueprintCallable, Category = "Building")
    bool CanPlaceBuilding(
        ADwarfUndergroundBaseTile* OriginTile,
        int32 Width,
        int32 Height);

    // NavMesh 업데이트 함수
    UFUNCTION(BlueprintCallable, Category = "Navigation")
    void UpdateNavMeshAroundLocation(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void ForceUpdateAllTilePositions();

private:
    // 유틸리티 함수
    bool IsValidLevelIndex(int32 LevelIndex);
    TSubclassOf<ADwarfUndergroundBaseTile> GetTileClassToUse();
    TArray<FVector2D> GenerateRectangularPattern(int32 Width, int32 Height);

    /** 해당 위치 바로 아래에 실제 지형(StaticMesh·Landscape 등)이 있는지 검사 */
    bool HasFloorBelow(const FVector& Location) const;

    bool IsBlockedByOtherActors(const FVector& Location,
        const FVector& HalfExtent = FVector(95.f, 80.f, 5.f)) const;

    bool HasBuildingOnTile(ADwarfUndergroundBaseTile* Tile) const;
};