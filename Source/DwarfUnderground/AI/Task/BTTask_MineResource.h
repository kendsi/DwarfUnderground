// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MineResource.generated.h"

UCLASS()
class DWARFUNDERGROUND_API UBTTask_MineResource : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_MineResource();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    // 블랙보드 키
    const FName TargetResourceKey = TEXT("TargetResource");
    const FName HasResourceKey = TEXT("HasResource");
};
