#include "BTService_UpdateTarget.h"
#include "DwarfUnderground/AI/Controller/UnitAIController.h"
#include "DwarfUnderground/Unit/Unit.h"
#include "DwarfUnderground/Unit/Squad/SquadComponent.h"
#include "DwarfUnderground/Unit/Squad/SquadProxy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateTarget::UBTService_UpdateTarget()
{
}

void UBTService_UpdateTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8*, float)
{
    auto* AICon = Cast<AUnitAIController>(OwnerComp.GetAIOwner());
    if (!AICon) return;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    /*if (AUnit* Self = Cast<AUnit>(AICon->GetPawn())) {
        if (Self->GetUnitState() == EUnitState::InSquad) {
            if (BB->GetValueAsObject(TargetUnitKey) == nullptr) {
                BB->SetValueAsBool(HasTargetKey, false);
            }
            else {
                return;
            }
        }
    }*/

    // 1. 현재 감지된 적 목록 얻기
    TArray<AActor*> Actors;
    AICon->GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), Actors);

    AUnit* Best = nullptr;
    float BestSq = FLT_MAX;
    const FVector SelfLoc = AICon->GetPawn()->GetActorLocation();

    // 2. 가장 가까운 적 찾기
    for (AActor* A : Actors)
    {
        AUnit* U = Cast<AUnit>(A);
        if (!AICon->IsEnemy(U) || U->IsDead()) {
            continue;
        }

        const float DistSq = FVector::DistSquared(SelfLoc, U->GetActorLocation());

        if (DistSq < BestSq) { 
            BestSq = DistSq; 
            Best = U;
        }
    }

    // 3. BB에 반영
    if (Best) {
        BB->SetValueAsObject(TargetUnitKey, Best);
        BB->SetValueAsBool(HasTargetKey, true);
        if (Best->SquadRef && !Best->SquadRef->Proxy->bIsStatic)
            BB->SetValueAsObject(TargetSquadKey, Best->SquadRef->GetOwner());
    }
    else {
        if (AUnit* CurrentEnemy = Cast<AUnit>(BB->GetValueAsObject(TargetUnitKey))) {
            if (CurrentEnemy->IsDead() || !IsValid(CurrentEnemy)) {
                BB->ClearValue(TargetUnitKey);
                BB->SetValueAsBool(IsChargingKey, false);
                BB->SetValueAsBool(HasTargetKey, false);
                if (AICon->GetUnitState() == EUnitState::Combat) {
                    AUnit* Self = Cast<AUnit>(AICon->GetPawn());
                    AICon->SetUnitState(Self->GetPrevState());
                }
            }
        }
    }
}