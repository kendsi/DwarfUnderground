// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DwarfListElement.generated.h"

class ADwarfUnit;
class UDwarfListElement;
/**
* DwarfListElement
	1. 드워프가 할당되어있고, 그에 따라 뭔가 보여줌
	2. 삭제 버튼 Delegate가 존재
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDwarfRemoveRequested, UDwarfListElement*,  Widget, ADwarfUnit*,TargetDwarf);
UCLASS()
class DWARFUNDERGROUND_API UDwarfListElement : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable,BlueprintCallable);
	FOnDwarfRemoveRequested OnDwarfRemoveRequested;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DwarfListElement", meta = (ExposeOnSpawn="true"))
	ADwarfUnit* DwarfUnit;
};
