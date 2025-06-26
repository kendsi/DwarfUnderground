// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "DwarfUnderground/DwarfUndergroundPlayerController.h"
#include "InfoPanel.generated.h"

class UAssignableDwarfList;
class ADwarfUnit;
/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API UInfoPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	ADwarfUndergroundPlayerController* PlayerController;

	//TODO: DwarfSelectionList 블루프린트화하면 템플릿 타입 변경
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UAssignableDwarfList> AssignableDwarfListClass;

public:
	void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void ClosePanel();

	UFUNCTION(BlueprintCallable)
	void SetTargetActor(AActor* TargetBuilding);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetActorSet(AActor* Target);

	//TODO: 나중에 UI 커지면 HUD쪽으로 옮기는 것 목표
	UFUNCTION(BlueprintCallable)
	void OpenAssignableDwarfList();

	//드워프 선택 리스트 생성 후, 그 리스트에서 드워프가 선택되었을 때 호출됨
	UFUNCTION(BlueprintImplementableEvent, Category="Dwarf")
	void OnDwarfSelected();

	//내 드워프 listClass에게 드워프 추가해달라고 요청
	UFUNCTION(BlueprintCallable, Category="Dwarf")
	void RequestAddDwarf(ADwarfUnit* DwarfUnit);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess="true"))
	AActor* TargetActor;
	
};
