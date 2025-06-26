// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipmentBase.generated.h"

struct FUnitStatModifier;

UCLASS()
class DWARFUNDERGROUND_API AEquipmentBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AEquipmentBase();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FName EquipmentName;

	//아이템 장착 시도 시 DB에서 아이템 데이터 로드를 위해 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	FName ItemID;

	//장착 시 상승하는 능력치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FUnitStatModifier> StatModifiers;

	// 장비를 장착했을 때 실행
	virtual void OnEquipped(class AUnit* Unit);

	// 장비를 해제했을 때 실행
	virtual void OnUnequipped(class AUnit* Unit);

};
