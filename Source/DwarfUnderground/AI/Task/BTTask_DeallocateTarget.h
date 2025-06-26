#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DeallocateTarget.generated.h"

UCLASS()
class DWARFUNDERGROUND_API UBTTask_DeallocateTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DeallocateTarget();

protected:

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	const FName HasTargetKey = TEXT("HasTarget");
};
