// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FormationData.h"
#include "SquadComponent.generated.h"

class AUnit;
class AActor;
class ASquadProxy;

UCLASS( ClassGroup=(Squad), meta=(BlueprintSpawnableComponent) )
class DWARFUNDERGROUND_API USquadComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USquadComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Squad")
	TArray<AUnit*> Units;

	/*UPROPERTY(BlueprintReadWrite)
	AUnit* Leader;*/

	UPROPERTY(BlueprintReadWrite, Category = "Squad")
	ASquadProxy* Proxy = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squad")
	FFormationData Formation;

	UFUNCTION(BlueprintCallable, Category = "Squad")
	void AddUnit(AUnit* Unit);

	UFUNCTION(BlueprintCallable, Category = "Squad")
	void RemoveUnit(AUnit* Unit);

	// 플레이어 명령
	UFUNCTION(BlueprintCallable, Category = "Squad")
	void ForceMove(const FFormationData& Data);
	UFUNCTION(BlueprintCallable, Category = "Squad")
	void IssueMove(const FFormationData& Data);
	void IssueAttack(AActor* Target);

	//void SetNewLeader(AUnit* Died);

	FVector GetCenter() const;

	UFUNCTION(BlueprintPure, Category = "Squad")
	int32 GetSquadNumber() const;

	UFUNCTION(BlueprintCallable, Category = "Squad")
	void SetSquadNumber(int32 Num);

	FFormationData GetLastFormation();

	//void UpdateFormationFollow();
	
	UPROPERTY()
	TMap<AUnit*, FVector> RelativeOffsets;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetEnemySquad(USquadComponent* EnemySquad);
	void SetEnemyUnit(AUnit* EnemyUnit);
	void CalcSlots(TArray<FVector>& Out) const;
	void CalcSlots(TArray<FVector>& Out, const FFormationData& Src) const;

private:

	int32 SquadNumber;

	FTimerHandle FormationMoveTimer;

	FName HasTargetKey = TEXT("HasTarget");
	FName TargetSquadKey = TEXT("TargetSquad");
	FName TargetUnitKey = TEXT("TargetUnit");
	FName IsLeaderKey = TEXT("IsLeader");
};

