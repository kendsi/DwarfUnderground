#include "BTTask_StopMoveInRange.h"
#include "DwarfUnderground/Unit/Unit.h"
#include "DwarfUnderground/AI/Controller/UnitAIController.h"
#include "DwarfUnderground/Unit/Squad/SquadComponent.h"
#include "DwarfUnderground/Component/UnitStatComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_StopMoveInRange::UBTTask_StopMoveInRange()
{
    NodeName = TEXT("Stop Move If In Range");
}

EBTNodeResult::Type UBTTask_StopMoveInRange::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp, uint8*)
{
    AUnitAIController* AICon = Cast<AUnitAIController>(OwnerComp.GetAIOwner());
    AUnit* Self = Cast<AUnit>(AICon->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    if (!Self || !BB) return EBTNodeResult::Failed;

    AUnit* Target = Cast<AUnit>(BB->GetValueAsObject(TargetUnitKey));
    float AttackRange = Self->Stats->GetFinalStat(EUnitStatType::AttackRange);

    if (!Target || AttackRange <= 0.f) return EBTNodeResult::Failed;

    const float DistSq = FVector::DistSquared(Self->GetActorLocation(), Target->GetActorLocation());
    if (DistSq <= FMath::Square(AttackRange) + 1000.0f)
    {
        //AICon->StopMovement();
        BB->SetValueAsBool(IsEnemyNearKey, true);
        return EBTNodeResult::Succeeded;
    }
    BB->SetValueAsBool(IsEnemyNearKey, false);
    return EBTNodeResult::Failed;
}