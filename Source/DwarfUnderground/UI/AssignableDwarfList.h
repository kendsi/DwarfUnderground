// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AssignableDwarfList.generated.h"

/**
 * 건물, 부대 등에 드워프를 할당하기 위한 리스트 UI를 나타냅니다.
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDwarfSelectedDelegate, ADwarfUnit*, SelectedDwarf);

UCLASS()
class DWARFUNDERGROUND_API UAssignableDwarfList : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDwarfSelectedDelegate OnDwarfSelected;
};
