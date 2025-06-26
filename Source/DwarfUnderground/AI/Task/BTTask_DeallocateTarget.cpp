// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DeallocateTarget.h"
#include "DwarfUnderground/Unit/Unit.h"
#include "DwarfUnderground/AI/Controller/UnitAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_DeallocateTarget::UBTTask_DeallocateTarget()
{
    NodeName = TEXT("Deallocate Target");
}

EBTNodeResult::Type UBTTask_DeallocateTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
    if (AUnitAIController* AICon = Cast<AUnitAIController>(OwnerComp.GetAIOwner())) {
        auto* BB = OwnerComp.GetBlackboardComponent();
        BB->SetValueAsBool(HasTargetKey, false);
        AICon->SetUnitState(EUnitState::Idle);
        AICon->GetPerceptionComp()->ForgetAll();
        AICon->GetClosePerceptionComp()->ForgetAll();
        return EBTNodeResult::Succeeded;
    }
    return EBTNodeResult::Failed;
}