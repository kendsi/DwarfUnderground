// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_PickNextPos.generated.h"

UCLASS()
class DWARFUNDERGROUND_API UBTService_PickNextPos : public UBTService
{
    GENERATED_BODY()

public:
    UBTService_PickNextPos();

protected:
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp,    
        uint8* NodeMemory, float DeltaSeconds) override;

    /** 편의용 키 */
    const FName CampLocationKey = TEXT("CampLocation");
    const FName NextPosKey = TEXT("NextPos");

    /** 기본 배회 반경 (에디터에서 덮어쓰기 가능) */
    UPROPERTY(EditAnywhere, Category = "AI")
    float WanderRadius = 800.f;
};