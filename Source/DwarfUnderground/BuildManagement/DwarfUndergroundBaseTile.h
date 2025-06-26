#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "DwarfUnderground/Interface/Buildable.h"
#include "./MapBase/Soil.h"
#include "Buildings/BuildingBase.h"
#include "Engine/StaticMesh.h"
#include "DwarfUndergroundBaseTile.generated.h"

class ADwarfUndergroundBuildManager;
class AStaticMeshActor;

UCLASS()
class DWARFUNDERGROUND_API ADwarfUndergroundBaseTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADwarfUndergroundBaseTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeshActor")
	UStaticMeshComponent* HexTile;

	// 이 타일에 건물이 지어져 있는지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building")
	bool bIsOccupied = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Material")
	UMaterialInstanceDynamic* DynamicMaterial_BaseTile;

	// 타일 위에 건물을 짓는 함수
	UFUNCTION(BlueprintCallable, Category = "Building")
	bool BuildOnTile(TSubclassOf<AActor> InType);

	// 타일 위의 건물을 없애는 함수
	UFUNCTION(BlueprintCallable)
	bool RemoveBuilding();

	UFUNCTION(BlueprintCallable, Exec)
	void SetTileOpacity(float InOpacity);

	UFUNCTION()
	void OnTileBeginCursorOver(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnTileEndCursorOver(UPrimitiveComponent* TouchedComponent);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	FLinearColor NormalColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	FLinearColor BuildModeColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
	FLinearColor DestroyModeColor = FLinearColor::Red;

	UPROPERTY()
	AStaticMeshActor* PreviewBuilding;

	UPROPERTY(EditDefaultsOnly, Category = "Building")
	UMaterialInterface* PreviewBuildingMat;

	void ShowPreview();
	void HidePreview();

	// 이 타일 위에 쌓여 있는 Soil들을 스택(혹은 배열) 형태로 관리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Soil")
	TArray<ASoil*> SoilStack;

	// Soil 한 층 추가
	UFUNCTION(BlueprintCallable, Category = "Soil")
	bool AddSoil(ASoil* NewSoil);

	// Soil 제거
	UFUNCTION(BlueprintCallable, Category = "Soil")
	bool RemoveSoil(ASoil* SoilToRemove);

	float CalculateTopSoilZ() const;

	void UpdateTilePosition();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soil")
	float BaseStoneZ = 0.f;

	// 그리드 좌표
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	int32 GridX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	int32 GridY;

	UPROPERTY(EditDefaultsOnly, Category = "Building")
	UMaterialInterface* InvalidPreviewBuildingMat;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building")
	TScriptInterface<IBuildable> OccupyingBuilding = nullptr;	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Building")
	TScriptInterface<IBuildable> PlacedBuilding = nullptr;
};
