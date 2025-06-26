#include "BTTask_PickNextPos.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_PickNextPos::UBTTask_PickNextPos()
{
    NodeName = TEXT("Next Pos");
    WanderRadius = 800.f;
}

EBTNodeResult::Type UBTTask_PickNextPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
    auto* BB = OwnerComp.GetBlackboardComponent();
    const FVector Center = BB->GetValueAsVector(CampLocationKey);

    if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        FNavLocation Out;
        if (Nav->GetRandomReachablePointInRadius(Center, WanderRadius, Out))
        {
            BB->SetValueAsVector(NextPosKey, Out.Location);
            float RandomDeviation = 5.0f;
            const float Rand = FMath::FRandRange(-RandomDeviation, RandomDeviation);
            BB->SetValueAsFloat(PatrolCooldownKey, 15.0f + Rand);
        }
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}