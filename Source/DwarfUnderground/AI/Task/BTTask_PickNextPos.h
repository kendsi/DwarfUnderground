#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PickNextPos.generated.h"

UCLASS()
class DWARFUNDERGROUND_API UBTTask_PickNextPos : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_PickNextPos();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    // 블랙보드 키
    const FName CampLocationKey = TEXT("CampLocation");
    const FName NextPosKey = TEXT("NextPos");
    const FName PatrolCooldownKey = TEXT("PatrolCooldown");

    // 기본 배회 반경 (에디터에서 덮어쓰기 가능)
    UPROPERTY(EditAnywhere, Category = "AI")
    float WanderRadius;
};
