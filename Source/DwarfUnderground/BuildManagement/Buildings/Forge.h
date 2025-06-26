// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "Forge.generated.h"

class UDwarfManagementComponent;
/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API AForge : public ABuildingBase
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Forge")
	bool TryManufactureItem(FName ItemID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Forge")
	float GetManufactureProgress(const ADwarfUnit* TargetDwarf) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Forge")
	FName GetManufacturingItem(const ADwarfUnit* TargetDwarf) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Forge")
	bool IsManufacturing(const ADwarfUnit* TargetDwarf) const;

	UFUNCTION(BlueprintCallable, Category="Forge")
	void CancelManufacturing(const ADwarfUnit* TargetDwarf);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	//작업중인 드워프, 진행율
	UPROPERTY()
	TMap<ADwarfUnit*, float> ManufacturingProgresses;
	//작업중인 드워프, 아이템
	UPROPERTY()
	TMap<ADwarfUnit*, FName> ManufacturingItems;

	//매니지먼트 컴포넌트
	UDwarfManagementComponent* Management;

	void CompleteManufacturing(const ADwarfUnit* TargetDwarf);
};