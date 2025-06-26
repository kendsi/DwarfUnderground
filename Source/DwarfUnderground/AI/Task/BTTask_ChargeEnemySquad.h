// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChargeEnemySquad.generated.h"

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API UBTTask_ChargeEnemySquad : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ChargeEnemySquad();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	const FName HasTargetKey = TEXT("HasTarget");
	const FName TargetUnitKey = TEXT("TargetUnit");
	const FName TargetSquadKey = TEXT("TargetSquad");
	const FName AttackRangeKey = TEXT("AttackRange");
	const FName IsChargingKey = TEXT("IsCharging");
};