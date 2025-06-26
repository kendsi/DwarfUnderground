#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WorkBuilding.generated.h"

UCLASS()
class DWARFUNDERGROUND_API UBTTask_WorkBuilding : public UBTTaskNode
{
	GENERATED_BODY()

public:
    UBTTask_WorkBuilding();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    // 블랙보드 키
    FName WorkBuildingKey = TEXT("WorkBuilding");
    FName HasWorkBuildingKey = TEXT("HasWorkBuilding");
};
