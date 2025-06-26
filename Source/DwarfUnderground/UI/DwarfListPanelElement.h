// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "DwarfListPanelElement.generated.h"

/**
 * 
 */

//버튼 클릭에 대한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnElementClickedDelegate, ADwarfUnit*, TargetDwarf);

UCLASS()
class DWARFUNDERGROUND_API UDwarfListPanelElement : public UUserWidget
{
	GENERATED_BODY()

public:
	//버튼 클릭에 대한 델리게이트 인스턴스
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnElementClickedDelegate OnElementClicked;

public:
	void SetDwarf(ADwarfUnit* TargetDwarf);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dwarf")
	void OnSetDwarf(ADwarfUnit* MyDwarf);

	virtual void NativeConstruct() override;

protected:
	UFUNCTION(BlueprintCallable)
	void HandleElementButtonClicked();

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true));
	ADwarfUnit* Dwarf;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText DwarfName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	FText StatusMessage;
};
