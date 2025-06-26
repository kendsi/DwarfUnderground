#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StopMoveInRange.generated.h"


UCLASS()
class DWARFUNDERGROUND_API UBTTask_StopMoveInRange : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_StopMoveInRange();

protected:
    FName TargetUnitKey = TEXT("TargetUnit");
    FName AttackRangeKey = TEXT("AttackRange");
    FName IsEnemyNearKey = TEXT("IsEnemyNear");

    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};