// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadComponent.h"
#include "DwarfUnderground/Unit/Unit.h"
#include "SquadProxy.h"
#include "DwarfUnderground/AI/Controller/DwarfAIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "BrainComponent.h"

// Sets default values for this component's properties
USquadComponent::USquadComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
	SquadNumber = -1;
}

void USquadComponent::AddUnit(AUnit* Unit)
{
	if (!Unit || Units.Contains(Unit) || !IsValid(Unit)) return;

	//if (!Leader || !IsValid(Leader)) {
	//	Leader = Unit;
	//	if (AUnitAIController* LeaderCon = Cast<AUnitAIController>(Unit->GetController()))
	//	{
	//		UBlackboardComponent* BB = LeaderCon->GetBlackboardComponent();
	//		BB->SetValueAsBool(IsLeaderKey, true);
	//		/*if (UCrowdFollowingComponent* CrowdComp = LeaderCon->FindComponentByClass<UCrowdFollowingComponent>())
	//		{
	//			CrowdComp->SetCrowdSeparation(false);
	//		}*/
	//	}
	//}

	AUnitAIController* AICon = Cast<AUnitAIController>(Unit->GetController());
	if (!AICon)
	{
		UE_LOG(LogTemp, Error, TEXT("SquadComponenet:부대 추가 실패: 드워프에 AICon이 없습니다."));
		return;
	}

	ADwarfAIController* DAICon = Cast<ADwarfAIController>(AICon);
	if (DAICon) {
		DAICon->DeallocateResource();
		DAICon->DeallocateWorkBuilding();
	}

	if (Formation.Anchor == FVector::ZeroVector) {
		Formation.Anchor = Unit->GetActorLocation();
	}

	Units.Add(Unit);
	Unit->SquadRef = this;
	AICon->SetUnitState(EUnitState::InSquad);

	if (!Proxy)
	{
		Proxy = GetWorld()->SpawnActor<ASquadProxy>();
		Proxy->OwnerSquad = this;
	}
}

void USquadComponent::RemoveUnit(AUnit* Unit)
{
	if (!Unit || !Units.Contains(Unit) || !IsValid(Unit)) return;

	//if (Leader == Unit) {
	//	if (AUnitAIController* LeaderCon = Cast<AUnitAIController>(Unit->GetController()))
	//	{
	//		UBlackboardComponent* BB = LeaderCon->GetBlackboardComponent();
	//		BB->SetValueAsBool(IsLeaderKey, false);
	//		/*if (UCrowdFollowingComponent* CrowdComp = LeaderCon->FindComponentByClass<UCrowdFollowingComponent>())
	//		{
	//			CrowdComp->SetCrowdSeparation(true);
	//		}*/
	//	}
	//}

	AUnitAIController* AICon = Cast<AUnitAIController>(Unit->GetController());
	if (!AICon) return;

	Units.Remove(Unit);
	if (Unit)
	{
		Unit->SquadRef = nullptr;
		Unit->OnDeselected();
	}

	AICon->SetUnitState(Unit->GetPrevState());
	

	if (Units.Num() == 0 && Proxy) {
		Proxy->Destroy();
		Proxy = nullptr;
	}
}

FVector USquadComponent::GetCenter() const
{
	FVector Center = FVector::ZeroVector;
	int32 Count = 0;

	for (AUnit* Unit : Units) {
		if (IsValid(Unit)) {
			Center += Unit->GetActorLocation();
			Count++;
		}
	}

	Center = Center / Count;

	return Center;
}

//void USquadComponent::IssueMove(const FFormationData& Data)
//{
//	Formation = Data;
//	//GetWorld()->GetTimerManager().ClearTimer(FormationMoveTimer);
//
//	TArray<FVector> Slots;
//	CalcSlots(Slots);
//
//	int32 Index = Units.IndexOfByKey(Leader);
//
//	// 리더만 실제 이동
//	if (Leader && Units.Contains(Leader))
//	{
//		if (AUnitAIController* AICon = Cast<AUnitAIController>(Leader->GetController())) {
//			UBlackboardComponent* BB = AICon->GetBlackboardComponent();
//			BB->ClearValue(TargetSquadKey);
//			AICon->HighPriorityMove(Slots[Index]);
//		}
//	}
//
//	RelativeOffsets.Empty();
//	for (int32 i = 0; i < Units.Num(); ++i)
//	{
//		AUnit* U = Units[i];
//		if (!U || U == Leader) continue;
//
//		FVector Offset = Slots[i] - Slots[Index];
//		RelativeOffsets.Add(U, Offset);
//	}
//
//	// 타이머 등록 (0.5초마다 갱신)
//	//GetWorld()->GetTimerManager().SetTimer(FormationMoveTimer, this, &USquadComponent::UpdateFormationFollow, 0.5f, true);
//}

void USquadComponent::ForceMove(const FFormationData& Data)
{
	Formation = Data;

	TArray<FVector> Slots;
	CalcSlots(Slots);

	for (int32 i = 0; i < Units.Num(); ++i)
	{
		AUnit* U = Units[i];
		if (AUnitAIController* AICon = Cast<AUnitAIController>(U->GetController())) {
			AICon->HighPriorityMove(Slots[i]);
		}
	}
}
void USquadComponent::IssueMove(const FFormationData& Data)
{
	Formation = Data;

	TArray<FVector> Slots;
	CalcSlots(Slots);

	for (int32 i = 0; i < Units.Num(); ++i)
	{
		AUnit* U = Units[i];
		if (AUnitAIController* AICon = Cast<AUnitAIController>(U->GetController())) {
			AICon->MoveToLocation(Slots[i]);
		}
	}
}

void USquadComponent::IssueAttack(AActor* Target)
{
	for (AUnit* Unit : Units)
	{
		if (!IsValid(Unit)) continue;
		Unit->Attack(Cast<AUnit>(Target));
	}
}

void USquadComponent::CalcSlots(TArray<FVector>& Out) const
{
	const int32 N = Units.Num();
	const int32 Cols = FMath::Max<int32>(1, FMath::FloorToInt(Formation.FrontLineLength / Formation.Spacing));
	const int32 Rows = FMath::CeilToInt((float)N / Cols);
	const FVector Fwd = - Formation.ForwardDir;

	for (int32 r = 0; r < Rows; ++r)
		for (int32 c = 0; c < Cols; ++c)
		{
			const int32 Id = r * Cols + c; if (Id >= N) break;
			const FVector Pos = Formation.Anchor
				+ Fwd * (r * Formation.Spacing)
				+ Formation.RightDir * ((c - (Cols - 1) * 0.5f) * Formation.Spacing);
			Out.Add(Pos);
		}
}

void USquadComponent::CalcSlots(TArray<FVector>& Out, const FFormationData& Src) const
{
	const int32 N = Units.Num();
	const int32 Cols = FMath::Max<int32>(1, FMath::FloorToInt(Src.FrontLineLength / Src.Spacing));
	const int32 Rows = FMath::CeilToInt((float)N / Cols);
	const FVector Fwd = -Src.ForwardDir;

	for (int32 r = 0; r < Rows; ++r)
		for (int32 c = 0; c < Cols; ++c)
		{
			const int32 Id = r * Cols + c; if (Id >= N) break;
			const FVector Pos = Src.Anchor
				+ Fwd * (r * Src.Spacing)
				+ Src.RightDir * ((c - (Cols - 1) * 0.5f) * Src.Spacing);
			Out.Add(Pos);
		}
}
int32 USquadComponent::GetSquadNumber() const
{
	return SquadNumber;
}

void USquadComponent::SetSquadNumber(int32 num) {
	SquadNumber = num;
}

FFormationData USquadComponent::GetLastFormation() {
	return Formation;
}

void USquadComponent::SetEnemySquad(USquadComponent* EnemySquad) {
	
	for (AUnit* Unit : Units) {
		if (!IsValid(Unit)) continue;

		AUnitAIController* AICon = Cast<AUnitAIController>(Unit->GetController());
		UBlackboardComponent* BB = AICon->GetBlackboardComponent();

		BB->SetValueAsBool(HasTargetKey, true);
		BB->SetValueAsObject(TargetSquadKey, EnemySquad->GetOwner());
	}
}

void USquadComponent::SetEnemyUnit(AUnit* EnemyUnit) {
	for (AUnit* Unit : Units) {
		if (!IsValid(Unit)) continue;

		AUnitAIController* AICon = Cast<AUnitAIController>(Unit->GetController());
		UBlackboardComponent* BB = AICon->GetBlackboardComponent();

		BB->SetValueAsBool(HasTargetKey, true);
		BB->SetValueAsObject(TargetUnitKey, EnemyUnit);
	}
}

//void USquadComponent::UpdateFormationFollow()
//{
//	if (!Leader) return;
//
//	for (const auto& Elem : RelativeOffsets)
//	{
//		AUnit* Member = Elem.Key;
//		FVector Offset = Elem.Value;
//		if (!Member || Member == Leader) continue;
//
//		FVector TargetPos = Leader->GetActorLocation() + Offset;
//		if (FVector::DistSquared(Member->GetActorLocation(), TargetPos) > FMath::Square(50.f))
//		{
//			if (AUnitAIController* AICon = Cast<AUnitAIController>(Member->GetController()))
//			{
//				AICon->MoveToLocation(TargetPos);
//			}
//		}
//	}
//}

//void USquadComponent::SetNewLeader(AUnit* Died) {
//	if (Died == Leader) {
//		Leader = nullptr;
//		for (AUnit* Unit : Units) {
//			if (IsValid(Unit) && Unit != Died) {
//				Leader = Unit;
//				if (AUnitAIController* LeaderCon = Cast<AUnitAIController>(Unit->GetController()))
//				{
//					UBlackboardComponent* BB = LeaderCon->GetBlackboardComponent();
//					BB->SetValueAsBool(IsLeaderKey, true);
//					if (UCrowdFollowingComponent* CrowdComp = LeaderCon->FindComponentByClass<UCrowdFollowingComponent>())
//					{
//						CrowdComp->SetCrowdSeparation(false);
//					}
//				}
//				break;
//			}
//		}
//		return;
//	}
//}

void USquadComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Units.RemoveAll([](AUnit* U) { return !IsValid(U); });

	/*for (const auto& Elem : RelativeOffsets)
	{
		AUnit* Member = Elem.Key;
		FVector Offset = Elem.Value;
		if (!Member || Member == Leader || !IsValid(Member)) continue;

		if (AUnitAIController* AICon = Cast<AUnitAIController>(Member->GetController()))
		{
			if (!AICon->bIsEnemyNear) {
				FVector TargetPos = Leader->GetActorLocation() + Offset;
				AICon->MoveToLocation(TargetPos, 100.0f);
			}
		}
	}*/
}