// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DwarfUnderground/Interface/Buildable.h"
#include "DwarfUnderground/UI/BuildProgressBarWidget.h"
#include "GameFramework/Actor.h"
#include "BuildingBase.generated.h"

class UBuildingManagerSubsystem;
//주의: 현재 순환참조 중
class ADwarfUndergroundBaseTile;

UCLASS(Abstract)
class DWARFUNDERGROUND_API ABuildingBase : public AActor, public IBuildable
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BuildingMesh;

	// 마우스 오버 시 보여줄 머티리얼
	UPROPERTY(EditDefaultsOnly, Category = "Building")
	UMaterialInterface* DestroyPreviewMat;

	// Custom Depth Stencil 값 (외곽선 식별용)
	UPROPERTY(EditDefaultsOnly, Category = "Building|Outline")
	int32 OutlineStencilValue = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Building")
	FName BuildingID;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//각 빌딩별 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building", meta = (AllowPrivateAccess = "true"))
	FString BuildingName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 Width;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 Height;

	//로딩바 관련 변수
	float ProgressTimer;
	UWidgetComponent* ConstructionWidgetComponent;
	UBuildProgressBarWidget* ConstructionProgressBarWidget;

	// 타일에 올라가있는 경우 해당 타일들 저장
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	TArray<ADwarfUndergroundBaseTile*> OccupiedTiles;

	// 건물의 원래 머티리얼 저장 (Destroy Mode용)
	UPROPERTY()
	UMaterialInterface* OriginalMaterial;

	//빌딩 매니저 정보
	UBuildingManagerSubsystem* BuildingManager;

	// 선택 상태
	UPROPERTY()
	bool bIsSelected = false;

public:
	// Sets default values for this actor's properties
	ABuildingBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual FString GetBuildingName();

	//Buildable 인터페이스 구현
	virtual int32 GetWidth() const override;
	virtual int32 GetHeight() const override;
	virtual void AddOccupiedTile(ADwarfUndergroundBaseTile* Tile) override;
	virtual void RemoveOccupiedTile(ADwarfUndergroundBaseTile* Tile) override;
	virtual TArray<ADwarfUndergroundBaseTile*> GetOccupiedTiles() override;
	virtual AActor* GetActor() override;

	void SetWidth(int32 NewWidth);
	void SetHeight(int32 NewHeight);

	//건설 Destroy Mode 시 커서 관련 이벤트
	UFUNCTION()
	void OnDestroyBeginCursorOver(UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnDestroyEndCursorOver(UPrimitiveComponent* TouchedComponent);

	//건설 로딩바 관련
	void AttachConstructionWidgetComponent();
	void FindAndGetConstructionProgressBarComponent();

	//건물의 입구 위치를 받아옴. 입구가 없다면 ActorLocation을 반환
	UFUNCTION(BlueprintCallable, Category = "DwarfManagement")
	FVector GetEntrancePosition();

	// 선택 관련 함수들
	UFUNCTION(BlueprintCallable, Category = "Selection")
	void SetSelected(bool bSelected);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Selection")
	bool IsSelected() const { return bIsSelected; }

	//건물의 입구 위치(있을수도 없을수도 있음)
	TWeakObjectPtr<USceneComponent> Entrance;

};