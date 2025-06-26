// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BuildingManagerSubsystem.generated.h"

/**
 * 
 */

class ABuildingBase;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingListUpdated, const TArray<ABuildingBase*>&, DwarfList);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingConstructedDelegate, ABuildingBase*, ConstructedBuilding);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingDestructedDelegate, ABuildingBase*, DestructedBuilding);
UCLASS()
class DWARFUNDERGROUND_API UBuildingManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnBuildingConstructedDelegate OnBuildingConstructed;

	UPROPERTY(BlueprintAssignable)
	FOnBuildingDestructedDelegate OnBuildingDestructed;

	UPROPERTY(BlueprintAssignable)
	FOnBuildingListUpdated OnBuildingListUpdated;
	
public:
	UFUNCTION(BlueprintCallable, Category = "BuildingManager")
	virtual void RegisterBuilding(ABuildingBase* Building);
	UFUNCTION(BlueprintCallable, Category = "BuildingManager")
	virtual void UnregisterBuilding(ABuildingBase* Building);

	UFUNCTION(BlueprintCallable, Category = "BuildingManager")
	virtual TArray<ABuildingBase*> GetAllBuildings();

protected:
	TArray<ABuildingBase*> Buildings;	
};
