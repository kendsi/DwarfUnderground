// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "EnemyUnit.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAllEnemiesDead);

UCLASS()
class DWARFUNDERGROUND_API AEnemyUnit : public AUnit
{
	GENERATED_BODY()
	
public:
	AEnemyUnit();

	static int32 AliveCount;
	static FOnAllEnemiesDead OnAllEnemiesDead;

protected:
	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<AEquipmentBase> DefaultWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Equipment")
	TSubclassOf<AEquipmentBase> DefaultShieldClass;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
