#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Resource/ResourceBase.h"
#include "./BuildManagement/DwarfUndergroundBuildManager.h"
#include "DwarfUndergroundPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class ADwarfUndergroundBuild;
class UUnitInteractionComponent;
class UBuildInteractionComponent;
class USquadComponent;
class AUnit;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

//UI에 빌딩이 클릭되었을 때 이벤트 전달하기 위한 Custom Event Delegate
// - When a building is clicked on the UI, pass the event 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorClicked, AActor*, ClickedActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActorUnclicked);

UCLASS()
class ADwarfUndergroundPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADwarfUndergroundPlayerController();

	void IncreaseResource(FName ResourceType, int32 Amount);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_LClick_Unit;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_RClick_Unit;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_LClick_Build;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_RClick_Build;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_SquadSelect;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_BuildingRotation;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_DwarfInfo;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_Inventory;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_ModeChange;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_BuildDestroyModeChange;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IA_ESC;

	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* MC_Unit;
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* MC_Build;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UUnitInteractionComponent* UnitComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBuildInteractionComponent* BuildComp;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 WoodCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StoneCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GoldOreCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 IronOreCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GoldIngotCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 IronIngotCount;

	UPROPERTY(BlueprintReadOnly)
	TArray<USquadComponent*> SquadList;

public:
	UFUNCTION(BlueprintCallable, Exec)
	void SetBuildSelectionMode(int32 ModeSelect);
	UFUNCTION(BlueprintCallable, Exec)
	int32 GetBuildSelectionMode();

	int32 BuildSelectionMode;

	UFUNCTION(BlueprintCallable, Exec)
	void SetBuildingType(TSubclassOf<AActor> TypeSelect);
	TSubclassOf<AActor> GetBuildingType();

	TSubclassOf<AActor> BuildingType;

	//빌딩 클릭시 이벤트를 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnActorClicked OnActorClicked;

	//빌딩 클릭시 이벤트를 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnActorUnclicked OnActorUnclicked;

	ADwarfUndergroundBuildManager* GetBuildManagerRef();

	// 빈 부대를 만들고 반환
	UFUNCTION(BlueprintCallable, Category = "Squad")
	USquadComponent* CreateEmptySquad();

	// 기존 부대 파괴
	UFUNCTION(BlueprintCallable, Category = "Squad")
	void RemoveSquad(int32 SquadNum);

	// 유닛을 지정 부대에 추가
	UFUNCTION(BlueprintCallable, Category = "Squad")
	void AddUnitToSquad(int32 SquadNum, AUnit* Unit);

	// 유닛을 지정 부대에서 제거
	UFUNCTION(BlueprintCallable, Category = "Squad")
	void RemoveUnitFromSquad(int32 SquadNum, AUnit* Unit);

	UFUNCTION(BlueprintCallable, Category = "Squad")
	TArray<USquadComponent*> GetSquadList();

	UFUNCTION(BlueprintCallable, Category = "Building Selection")
	void SelectBuilding(AActor* Building);

	UFUNCTION(BlueprintCallable, Category = "Building Selection")
	void DeselectCurrentBuilding();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Building Selection")
	AActor* GetSelectedBuilding() const { return CurrentSelectedBuilding; }


private:
	UPROPERTY()
	ADwarfUndergroundBuildManager* BuildManagerRef = nullptr;

	UPROPERTY()
	class ABuildingBase* CurrentSelectedBuilding = nullptr;

	void HandleAllDead();
};