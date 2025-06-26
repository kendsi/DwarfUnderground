// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "DwarfUnderground/Unit/Unit.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack Target");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
    if (AAIController* AICon = OwnerComp.GetAIOwner())
    {
        if (AUnit* Self = Cast<AUnit>(AICon->GetPawn()))    
        {
            if (AUnit* Target = Cast<AUnit>(
                OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetUnitKey)))
            {
                if (Self->IsAttackCooldown()) {
                    return EBTNodeResult::Failed;
                }
                AICon->SetFocus(Target);
                Self->Attack(Target);
                return EBTNodeResult::Succeeded;
            }
        }
    }
    return EBTNodeResult::Failed;
}