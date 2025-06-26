// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "UnitAIController.generated.h"

enum class EUnitState : uint8;

UCLASS()
class DWARFUNDERGROUND_API AUnitAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AUnitAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UAIPerceptionComponent* GetPerceptionComp();
	UAIPerceptionComponent* GetClosePerceptionComp();

	void HighPriorityMove(const FVector dest);

	bool IsEnemy(const AActor* Target) const;

	void SetUnitState(EUnitState state);

	EUnitState GetUnitState();

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	// 지각 컴포넌트로 플레이어 감지
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* ClosePerceptionComp;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAISenseConfig_Sight* CloseSightConfig;

	virtual void SetupPerception();

	// 시야에 변화가 생겼을 때 호출
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stim);

	// Blackboard 키 캐시
	const FName HasOrderKey = TEXT("HasOrder");
	const FName HasTargetKey = TEXT("HasTarget");
	const FName TargetUnitKey = TEXT("TargetUnit");
	const FName TargetSquadKey = TEXT("TargetSquad");
	const FName HasResourceKey = TEXT("HasResource");
	const FName TargetResourceKey = TEXT("TargetResource");
	const FName AttackRangeKey = TEXT("AttackRange");
	const FName UnitStateKey = TEXT("UnitState");
	const FName IsEnemyNearKey = TEXT("IsEnemyNear");
	
	// 시야 잠깐 사라져도 delay 유지
	FTimerHandle ForgetHandle;
	float ForgetDelay;
};
