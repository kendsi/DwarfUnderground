// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InfoWidget.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "ManagedDwarfListWidget.generated.h"

class UDwarfManagementComponent;
/**
 *
 * TODO: DwarfListPanel과 합치기 (얘가 DwarfListPanel을 소유하는 형식으로)
 */
UCLASS()
class DWARFUNDERGROUND_API UManagedDwarfListWidget : public UInfoWidget
{
	GENERATED_BODY()

public:
	UDwarfManagementComponent* TargetDwarfManagementComponent;

public:
	virtual void SetTargetActor(AActor* Target) override;

	UFUNCTION(BlueprintCallable)
	void RerenderDwarves(const TArray<ADwarfUnit*>& DwarfList);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDwarfListChanged(const TArray<ADwarfUnit*>& DwarfList);

	UFUNCTION(BlueprintImplementableEvent)
	void RenderDwarfSlot(ADwarfUnit* Dwarf);
	
	UFUNCTION(BlueprintImplementableEvent)
	void RenderEmptyDwarfSlot();
	
};
