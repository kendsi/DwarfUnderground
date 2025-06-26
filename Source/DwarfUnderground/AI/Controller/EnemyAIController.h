// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitAIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class DWARFUNDERGROUND_API AEnemyAIController : public AUnitAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	const FName CampLocationKey = TEXT("CampLocation");
	const FName PatrolRadiusKey = TEXT("PatrolRadius");
	const FName PatrolCooldownKey = TEXT("PatrolCooldown");
	// 캠프 위치
	FVector CampLocation;
};
