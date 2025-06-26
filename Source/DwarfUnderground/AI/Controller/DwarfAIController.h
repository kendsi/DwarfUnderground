// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitAIController.h"
#include "DwarfUnderground/Resource/ResourceBase.h"
#include "DwarfAIController.generated.h"

class AResourceBase;
class AUnit;
class ABuildingBase;

UCLASS()
class DWARFUNDERGROUND_API ADwarfAIController : public AUnitAIController
{
	GENERATED_BODY()
	
public:
	ADwarfAIController();

	UFUNCTION(BlueprintCallable)
	void AllocateWorkBuilding(ABuildingBase* Building);

	UFUNCTION(BlueprintCallable)
	void DeallocateWorkBuilding();

	UFUNCTION(BlueprintCallable)
	void AllocateResource(AResourceBase* Resource);

	UFUNCTION(BlueprintCallable)
	void DeallocateResource();
	
	UFUNCTION(BlueprintCallable)
	void SetStorageLocation(FVector Location);

	UFUNCTION(BlueprintCallable)
	FVector GetStorageLocation();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	//virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	UPROPERTY()
	AResourceBase* TargetResource;

	UPROPERTY()
	FVector StorageLocation;

	const FName WorkBuildingKey = TEXT("WorkBuilding");
	const FName HasWorkBuildingKey = TEXT("HasWorkBuilding");
	const FName WorkBuildingLocationKey = TEXT("WorkBuildingLocation");
	const FName StorageLocationKey = TEXT("StorageLocation");
	const FName ResourceLocationKey = TEXT("ResourceLocation");

	UPROPERTY()
	TEnumAsByte<ECollisionEnabled::Type> CachedCollision;

	UPROPERTY()
	TObjectPtr<AUnit> AttackingTarget;

	float LastAttackTime;
};
