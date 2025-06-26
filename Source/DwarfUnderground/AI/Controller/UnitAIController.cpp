// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "NavigationSystem.h"
#include "DwarfUnderground/Unit/Unit.h"
#include "DwarfUnderground/Unit/Squad/FormationData.h"
#include "DwarfUnderground/Unit/Squad/SquadComponent.h"
#include "DwarfUnderground/Unit/Squad/SquadProxy.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DwarfUnderground/Component/UnitStatComponent.h"

AUnitAIController::AUnitAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	UCrowdFollowingComponent* CrowdFollowingComponent = FindComponentByClass<UCrowdFollowingComponent>();

	if (CrowdFollowingComponent)
	{
		CrowdFollowingComponent->SetCrowdSeparation(true);
		CrowdFollowingComponent->SetCrowdSeparationWeight(30.0f);
		CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(1.0f);
	}

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	ClosePerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("ClosePerceptionComp"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	CloseSightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("CloseSight"));
	SetGenericTeamId(FGenericTeamId(0)); // 0 = Player/Dwarf
	SetupPerception();

	ForgetDelay = 3.0f;
}

void AUnitAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AUnit* Unit = Cast<AUnit>(InPawn);
	if (Unit && Unit->BehaviorTreeAsset)
	{
		RunBehaviorTree(Unit->BehaviorTreeAsset);

		UBlackboardComponent* BB = GetBlackboardComponent();
		float UnitAttackRange = 0.0f;
		UnitAttackRange = Unit->Stats->GetFinalStat(EUnitStatType::AttackRange);

		BB->SetValueAsBool(HasOrderKey, false);
		BB->SetValueAsFloat(AttackRangeKey, UnitAttackRange - 30.0f);
		BB->SetValueAsEnum(UnitStateKey, static_cast<uint8>(Unit->GetUnitState()));
		BB->SetValueAsBool(IsEnemyNearKey, false);
	}
}

void AUnitAIController::HighPriorityMove(const FVector Dest) {
	UBlackboardComponent* BB = GetBlackboardComponent();
	BB->SetValueAsBool(HasOrderKey, true);
	MoveToLocation(Dest);
}

void AUnitAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.Code != EPathFollowingResult::Success)
		return;

	UBlackboardComponent* BB = GetBlackboardComponent();
	BB->SetValueAsBool(HasOrderKey, false);

	if (AUnit* U = Cast<AUnit>(GetPawn()))
	{
		if (USquadComponent* S = U->SquadRef)
		{
			// 스쿼드가 기억한 마지막 포메이션 데이터에서 ForwardDir 꺼내서 회전
			const FFormationData& Form = S->GetLastFormation();
			const FRotator WantRot = Form.ForwardDir.Rotation();
			U->StartSmoothRotation(WantRot);
		}
	}
}

void AUnitAIController::SetupPerception()
{
	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2500.0f;
	SightConfig->SetMaxAge(8.0f);
	SightConfig->PeripheralVisionAngleDegrees = 120.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// 360° 짧은 시야
	CloseSightConfig->SightRadius = 1000.f;
	CloseSightConfig->LoseSightRadius = 1100.f;
	CloseSightConfig->SetMaxAge(3.f);
	CloseSightConfig->PeripheralVisionAngleDegrees = 180.f;
	CloseSightConfig->DetectionByAffiliation.bDetectEnemies = true;
	CloseSightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	CloseSightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AUnitAIController::OnPerceptionUpdated);

	ClosePerceptionComp->ConfigureSense(*CloseSightConfig);
	ClosePerceptionComp->SetDominantSense(CloseSightConfig->GetSenseImplementation());
	ClosePerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AUnitAIController::OnPerceptionUpdated);
}

void AUnitAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stim)
{
	if (!IsEnemy(Actor)) return;

	const float KeepAggroDist = 600.f;

	UBlackboardComponent* BB = GetBlackboardComponent();
	AUnit* Self = Cast<AUnit>(GetPawn());
	AUnit* NewTarget = Cast<AUnit>(Actor);
	AUnit* CurrentTarget = Cast<AUnit>(BB->GetValueAsObject(TargetUnitKey));

	if (!IsValid(NewTarget) || NewTarget->IsDead())
	{
		return;
		/*if (GetUnitState() == EUnitState::Combat)
			SetUnitState(Self->GetPrevState());*/
	}

	if (Stim.WasSuccessfullySensed())
	{
		if (GetUnitState() != EUnitState::InSquad && GetUnitState() != EUnitState::Combat)
			SetUnitState(EUnitState::Combat);
		
		float AttackRange = Self->Stats->GetFinalStat(EUnitStatType::AttackRange);
		BB->SetValueAsFloat(AttackRangeKey, AttackRange - 50.0f);
		if (!CurrentTarget || CurrentTarget->IsDead())
		{
			BB->SetValueAsObject(TargetUnitKey, NewTarget);
			BB->SetValueAsBool(HasTargetKey, true);
			if (NewTarget->SquadRef && !NewTarget->SquadRef->Proxy->bIsStatic)
				BB->SetValueAsObject(TargetSquadKey, NewTarget->SquadRef);
		}
		else
		{
			const float CurDist = GetPawn()->GetDistanceTo(CurrentTarget);
			const float NewDist = GetPawn()->GetDistanceTo(NewTarget);

			if (NewDist < CurDist) // 더 가까우면 교체
			{
				BB->SetValueAsObject(TargetUnitKey, NewTarget);
				BB->SetValueAsBool(HasTargetKey, true);
				if (NewTarget->SquadRef && !NewTarget->SquadRef->Proxy->bIsStatic)
					BB->SetValueAsObject(TargetSquadKey, NewTarget->SquadRef);
			}
		}
	}
	else if (CurrentTarget == NewTarget)
	{
		const float Dist = GetPawn()->GetDistanceTo(NewTarget);
		if (Dist <= KeepAggroDist)
		{
			return;
		}
	}
}

bool AUnitAIController::IsEnemy(const AActor* Target) const
{
	if (const auto* TargetPawn = Cast<APawn>(Target))
	{
		if (const auto* ctrl = Cast<AUnitAIController>(TargetPawn->GetController()))
		{
			if (ctrl->GetGenericTeamId() == this->GetGenericTeamId())
				return false;
			return true;
		}
	}
	return false;
}

UAIPerceptionComponent* AUnitAIController::GetPerceptionComp()
{
	return PerceptionComp;
}

UAIPerceptionComponent* AUnitAIController::GetClosePerceptionComp()
{
	return ClosePerceptionComp;
}

void AUnitAIController::SetUnitState(EUnitState state)
{
	AUnit* Self = Cast<AUnit>(GetPawn());
	UBlackboardComponent* BB = GetBlackboardComponent();

	Self->SetUnitState(state);
	BB->SetValueAsEnum(UnitStateKey, static_cast<uint8>(state));
}

EUnitState AUnitAIController::GetUnitState()
{
	AUnit* Self = Cast<AUnit>(GetPawn());
	return Self->GetUnitState();
}
