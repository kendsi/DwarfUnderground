// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DwarfUnderground/BuildManagement/Buildings/BuildingBase.h"
#include "InfoWidget.generated.h"

/**
 * BuildingInfoPanel의 안쪽에 들어가는 위젯들
 */
UCLASS()
class DWARFUNDERGROUND_API UInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetTargetActor(AActor* Target);
	
	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor();

	UFUNCTION(BlueprintImplementableEvent)
	void OnBuildingSet(AActor* Target);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "InfoWidget")
	FString WidgetName;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* TargetActor;	
};
