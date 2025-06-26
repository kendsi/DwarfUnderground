// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/PanelWidget.h"
#include "DwarfListPanelElement.h"
#include "DwarfListPanel.generated.h"

class ADwarfUnit;
/**
 * 모든 드워프 리스트의 부모가 되는 패널
 *  - 아직 예전에 만든 리스트들은 부모가 아니다. 수정하면 좋을듯
 */
UCLASS()
class DWARFUNDERGROUND_API UDwarfListPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	TArray<ADwarfUnit*> Dwarves;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ElementContainer;
	
	UPROPERTY(BlueprintAssignable, Category = "UIEvent")
	FOnElementClickedDelegate OnElementClicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UDwarfListPanelElement> PanelElementClass;

public:
	UFUNCTION(BlueprintCallable, Category = "DwarfListPanel")
	void UpdateDwarfList(TArray<ADwarfUnit*> DwarfList);

	UFUNCTION(BlueprintCallable, Category = "DwarfListPanel")
	void RenderDwarfList();

	UFUNCTION(BlueprintNativeEvent, Category = "DwarfListPanel")
	void RenderDwarfListElement(ADwarfUnit* TargetDwarf, UDwarfListPanelElement* Element);
	
	
protected:
	UFUNCTION(BlueprintCallable)
	void HandleElementClicked(ADwarfUnit* TargetDwarf);

private:
	UDwarfListPanelElement* AddElementOnPanel();
	
};
