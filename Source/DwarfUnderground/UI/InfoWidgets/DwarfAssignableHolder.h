// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "DwarfAssignableHolder.generated.h"

/**
 * 드워프 리스트의 엘리먼트 등 드워프 할당-비할당 시 다른 요소가 나타나는 엘리먼트의 부모입니다.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDwarfAddRequestedDelegate);

UCLASS(Abstract)
class DWARFUNDERGROUND_API UDwarfAssignableHolder : public UUserWidget
{
	GENERATED_BODY()

public:

public:
	UFUNCTION(BlueprintCallable)
	void SetDwarf(ADwarfUnit* Dwarf);

	UFUNCTION(BlueprintCallable)
	void RemoveDwarf();

	UFUNCTION(BlueprintPure)
	ADwarfUnit* GetDwarf();

	UFUNCTION(BlueprintImplementableEvent, Category="DwarfWidget")
	void OnSetDwarf(ADwarfUnit* Dwarf);
	
	UFUNCTION(BlueprintImplementableEvent, Category="DwarfWidget")
	void OnRemoveDwarf();

	//TODO: 그냥 패널이나 건물 직접 할당 받아서 Cast하여 사용하는 방식이 나을듯?
	UPROPERTY(BlueprintAssignable, Category="DwarfWidget")
	FOnDwarfAddRequestedDelegate OnDwarfAddRequested;

protected:
	virtual void NativeConstruct() override;

	//드워프 할당 요청
	UFUNCTION(BlueprintCallable)
	void RequestAssignDwarf();
	
private:
	ADwarfUnit* TargetDwarf;
};
