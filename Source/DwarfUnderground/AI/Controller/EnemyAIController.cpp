// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "DwarfUnderground/Unit/EnemyUnit.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DwarfUnderground/Component/UnitStatComponent.h"
#include "Navigation/PathFollowingComponent.h"

AEnemyAIController::AEnemyAIController()
{
	SetGenericTeamId(FGenericTeamId(1)); // 1 = Enemy
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AEnemyUnit* EU = Cast<AEnemyUnit>(InPawn);

	if (EU && EU->BehaviorTreeAsset)
	{
		RunBehaviorTree(EU->BehaviorTreeAsset);

		UBlackboardComponent* BB = GetBlackboardComponent();
		// 일단 캠프 위치 = 액터 위치한 곳
		CampLocation = InPawn->GetActorLocation();
		float ModifiedAttackRange = EU->Stats->GetFinalStat(EUnitStatType::AttackRange);

		BB->SetValueAsVector(CampLocationKey, CampLocation);
		BB->SetValueAsFloat(PatrolRadiusKey, 800.f);
		BB->SetValueAsFloat(AttackRangeKey, ModifiedAttackRange);
		BB->SetValueAsFloat(PatrolCooldownKey, 5.0f);
	}
}
