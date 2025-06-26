// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitStatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnitStatChangedDelegate);

UENUM(BlueprintType)
enum class EUnitStatType: uint8
{
	// 유닛 스탯
	MaxHealth,
	AttackPower,
	Accuracy,
	Evasion,
	AttackRange,
	AttackCooldown,
	ArmorRating,
	Speed,

	//노동 관련 스탯들
	Mining,
	Recruiting,
	Building,
	Sales,
	Farming,
	Spadework,
	Cooking,

	//Enum 순회를 위한 값
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EUnitStatType, EUnitStatType::Count);

USTRUCT(BlueprintType)
struct DWARFUNDERGROUND_API FUnitStatModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUnitStatType StatType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AddModifier = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MulModifier = 0.0f;
};

//아이템 등에 의한 능력치 증가량을 Map 형태로 저장중인데, Map의 Value에는 Array를 저장할 수 없음. 이에 따른 Wrapper Class
USTRUCT(BlueprintType)
struct DWARFUNDERGROUND_API FUnitStatModifierStack
{
	GENERATED_BODY()
	UPROPERTY()
	TArray<FUnitStatModifier> Modifiers;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DWARFUNDERGROUND_API UUnitStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//아이템 등 추가 수치 적용이 완료된 스탯을 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetFinalStat(EUnitStatType StatType) const;

	//아이템 등 추가 수치 적용이 완료된 모든 스탯을 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "Stats")
	TMap<EUnitStatType, float> GetAllFinalStats() const;

	//추가 스탯을 적용합니다. 삭제 시 필요한 ID값을 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 AddModifiers(TArray<FUnitStatModifier> Modifiers);
	
	//추가 스탯을 적용합니다. 삭제 시 필요한 ID값을 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "Stats")
	int32 AddModifier(FUnitStatModifier Modifier);

	//추가된 스탯을 삭제합니다. 삭제 성공 여부를 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool RemoveModifiers(int32 Id);

protected:
	UPROPERTY(EditAnywhere, Category = "Stats")
	TMap<EUnitStatType, float> BaseStats;

	//현재 장착된 아이템, 버프 등 모디파이어 정보를 저장(ID, Modifier)
	UPROPERTY(VisibleAnywhere, Category = "Stats")
	TMap<int32, FUnitStatModifierStack> ActiveModifiers;

	int32 LastModifierID;

	UPROPERTY(BlueprintAssignable, Category = Stats)
	FOnUnitStatChangedDelegate OnUnitStatChanged;
		
};
