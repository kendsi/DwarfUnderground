#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DepositResource.generated.h"

UCLASS()
class DWARFUNDERGROUND_API UBTTask_DepositResource : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_DepositResource();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    // 블랙보드 키
    const FName HasResourceKey = TEXT("HasResource");
};
