#pragma once
#include "../../Interface/Buildable.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Soil.generated.h"

// 단순화된 흙 타입 - Normal만 유지
UENUM(BlueprintType)
enum class ESoilType : uint8
{
    Normal UMETA(DisplayName = "Normal")
};

UCLASS()
class DWARFUNDERGROUND_API ASoil : public AActor, public IBuildable
{
    GENERATED_BODY()
public:
    ASoil();
protected:
    virtual void BeginPlay() override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    int32 Width;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    int32 Height;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    TArray<ADwarfUndergroundBaseTile*> OccupiedTiles;
public:
    virtual void Tick(float DeltaTime) override;

    // IBuildable 구현부
    virtual int32 GetWidth() const override;
    virtual int32 GetHeight() const override;
    virtual void AddOccupiedTile(ADwarfUndergroundBaseTile* Tile) override;
    virtual void RemoveOccupiedTile(ADwarfUndergroundBaseTile* Tile) override;
    virtual TArray<ADwarfUndergroundBaseTile*> GetOccupiedTiles() override;
    virtual AActor* GetActor() override;

    // 기본 흙 메쉬 컴포넌트만 유지
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Soil")
    UStaticMeshComponent* SoilMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soil")
    float SoilHeight = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soil")
    ESoilType SoilType = ESoilType::Normal;

    // 기본 흙 설정 함수
    UFUNCTION(BlueprintCallable, Category = "Soil")
    void SetSoilType(ESoilType NewType);

    // 메시와 머티리얼 레퍼런스
    UPROPERTY(EditDefaultsOnly, Category = "Soil")
    UStaticMesh* NormalSoilMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Soil")
    UMaterialInterface* NormalSoilMaterial;
};