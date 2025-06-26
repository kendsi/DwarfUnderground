// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChargeEnemySquad.h"
#include "DwarfUnderground/AI/Controller/UnitAIController.h"
#include "DwarfUnderground/Unit/Unit.h"
#include "DwarfUnderground/Unit/Squad/SquadProxy.h"
#include "DwarfUnderground/Unit/Squad/SquadComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ChargeEnemySquad::UBTTask_ChargeEnemySquad() {

}

EBTNodeResult::Type UBTTask_ChargeEnemySquad::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*) {
    if (AUnitAIController* AICon = Cast<AUnitAIController>(OwnerComp.GetAIOwner()))
    {
        if (AUnit* Self = Cast<AUnit>(AICon->GetPawn())) {
            if (Self->SquadRef) {
                UBlackboardComponent* BB = AICon->GetBlackboardComponent();
                if (ASquadProxy* EnemySquadProxy = Cast<ASquadProxy>(BB->GetValueAsObject(TargetSquadKey))) {
                    USquadComponent* EnemySquad = EnemySquadProxy->OwnerSquad;
                    if (EnemySquad) {
                        FVector EnemyCenter = EnemySquad->GetCenter();

                        FFormationData Form;
                        Form.Anchor = EnemyCenter;
                        Form.ForwardDir = (EnemyCenter - Self->SquadRef->GetCenter()).GetSafeNormal2D();
                        Form.RightDir = FVector::CrossProduct(FVector::UpVector, Form.ForwardDir).GetSafeNormal();
                        Form.FrontLineLength = Self->SquadRef->Formation.FrontLineLength;
                        Form.Spacing = Self->SquadRef->Formation.Spacing;

                        for (AUnit* U : Self->SquadRef->Units) {
                            if (U && IsValid(U)) {
                                AUnitAIController* A = Cast<AUnitAIController>(U->GetController());
                                A->GetBlackboardComponent()->SetValueAsBool(IsChargingKey, true);
                            }
                        }
                        Self->SquadRef->IssueMove(Form);
                        return EBTNodeResult::Succeeded;
                    }
                }
            }
        }
    }
    return EBTNodeResult::Failed;
}