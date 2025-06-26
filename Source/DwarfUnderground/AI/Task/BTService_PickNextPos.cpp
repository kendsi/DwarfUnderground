// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PickNextPos.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTService_PickNextPos::UBTService_PickNextPos()
{
    NodeName = TEXT("Pick Random Wander Pos");
    Interval = 0.1f;   // 0.1초마다
    RandomDeviation = 0.f;
    bNotifyBecomeRelevant = false;
}

void UBTService_PickNextPos::TickNode(UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory, float /*DeltaSeconds*/)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    const FVector Center = BB->GetValueAsVector(CampLocationKey);

    if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        FNavLocation Out;
        if (Nav->GetRandomReachablePointInRadius(Center, WanderRadius, Out))
        {
            BB->SetValueAsVector(NextPosKey, Out.Location);
        }
    }
}