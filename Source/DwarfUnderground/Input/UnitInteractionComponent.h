// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DwarfUnderground/Unit/Squad/FormationData.h"
#include "EnhancedInputComponent.h"
#include "UnitInteractionComponent.generated.h"

class UInputAction;
class USquadComponent;
class ASelectionHUD;
class ADwarfUndergroundPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnPreviewFormation,
	const TArray<FFormationData>&, PreviewForms,
	const TArray<USquadComponent*>&, Squads,
	bool, bFinal);

UCLASS( ClassGroup=(Input), meta=(BlueprintSpawnableComponent) )
class DWARFUNDERGROUND_API UUnitInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UUnitInteractionComponent();

	UPROPERTY(BlueprintAssignable)
	FOnPreviewFormation OnPreviewFormation;

	void Bind(class UEnhancedInputComponent* EIC, const UInputAction* LClick, const UInputAction* RClick,
		const UInputAction* Selection, const UInputAction* Info, const UInputAction* Inventory, const UInputAction* ModeChange, const UInputAction* ESC);
	
	UFUNCTION(BlueprintCallable)
	void SelectSquad(int32 SquadNum);

	//UI에서 사용하기 위한 현재 선택된 부대 정보
	UFUNCTION(BlueprintPure, BlueprintCallable)
	const TArray<USquadComponent*>& GetCurrentSelectedSquad();

private:

	/* 좌/우 클릭·드래그 */
	void LDragStart();
	void LDragEnd();
	void RDragStart();
	void RDragEnd();
	void SelectSquad(const FInputActionValue& Value);
	void ShowDwarfInfo();
	void ShowInventory();
	void HandleModeChange();
	void HandleESC();

	// 클릭 Threshold
	float ClickThreshold;
	float PixelToWorldScale;

	ASelectionHUD* HUDRef;
	ADwarfUndergroundPlayerController* CachedPC = nullptr;

	// 선택 상태
	bool bLDrag = false, bRDrag = false;
	FVector2D LS, LE, RS, RE;
	TArray<USquadComponent*> CurrSel;
	

	// 헬퍼
	void GatherSquads(const FVector2D& A, const FVector2D& B, TArray<USquadComponent*>& Out) const;

	virtual void BeginPlay() override;
	virtual void TickComponent(float Dt, ELevelTick, FActorComponentTickFunction*) override;

	FFormationData CalcFormationFromScreenLine(const FVector2D& ScreenA, const FVector2D& ScreenB) const;
	void HandleSelect(const TArray<USquadComponent*>& Squads);
	void HandleMove(const TArray<USquadComponent*>& Squads, const FVector& Dest);
	void HandleFormMove(const TArray<USquadComponent*>& Squads, const FFormationData& Form);
};
