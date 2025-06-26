#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateTarget.generated.h"

UCLASS()
class DWARFUNDERGROUND_API UBTService_UpdateTarget : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8*, float) override;

	// 블랙보드 키
	const FName TargetUnitKey = TEXT("TargetUnit");
	const FName TargetSquadKey = TEXT("TargetSquad");
	const FName HasTargetKey = TEXT("HasTarget");
	const FName IsChargingKey = TEXT("IsCharging");
	const FName UnitStateKey = TEXT("UnitState");
};
