// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitStatComponent.h"

#include "DwarfUnderground/Unit/Unit.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UUnitStatComponent::UUnitStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;	

	//기본 스탯 초기화
	// - 만약 나중에 데이터 등에서 불러온다면 여기서 처리하면 될듯?
	BaseStats.Add(EUnitStatType::MaxHealth, 100.0f);
	BaseStats.Add(EUnitStatType::AttackPower, 10.0f);
	BaseStats.Add(EUnitStatType::Accuracy, 200.0f);
	BaseStats.Add(EUnitStatType::Evasion, 20.0f);
	BaseStats.Add(EUnitStatType::AttackRange, 200.0f);
	BaseStats.Add(EUnitStatType::AttackCooldown, 2.0f);
	BaseStats.Add(EUnitStatType::ArmorRating, 0.0f);
	BaseStats.Add(EUnitStatType::Speed, 0.0f);
	BaseStats.Add(EUnitStatType::Mining, 10.0f);
	BaseStats.Add(EUnitStatType::Recruiting, 10.0f);
	BaseStats.Add(EUnitStatType::Building, 10.0f);
	BaseStats.Add(EUnitStatType::Sales, 10.0f);
	BaseStats.Add(EUnitStatType::Farming, 10.0f);
	BaseStats.Add(EUnitStatType::Spadework, 10.0f);
	BaseStats.Add(EUnitStatType::Cooking, 10.0f);
}


// Called when the game starts
void UUnitStatComponent::BeginPlay()
{
	Super::BeginPlay();

	//기본 이동속도 가져옴
	if (AActor* Actor = GetOwner())
	{
		if (AUnit* Character = Cast<AUnit>(Actor))
		{
			BaseStats[EUnitStatType::Speed] = Character->GetCharacterMovement()->MaxWalkSpeed + BaseStats[EUnitStatType::Speed];
			Character->GetCharacterMovement()->MaxWalkSpeed = BaseStats[EUnitStatType::Speed];
		}
	}
	
}


// Called every frame
void UUnitStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UUnitStatComponent::GetFinalStat(EUnitStatType StatType) const
{
	float StatSum = BaseStats[StatType];
	float AddSum = 0.0f;
	float MulSum = 0.0f;
	for (auto& Stack: ActiveModifiers)
	{		
		for (auto& Modifier: Stack.Value.Modifiers)
		{
			if (Modifier.StatType == StatType)
			{
				AddSum += Modifier.AddModifier;
				MulSum += Modifier.MulModifier;
			}
		}
	}
	StatSum += AddSum;
	StatSum += StatSum * MulSum;
	return StatSum;
}

TMap<EUnitStatType, float> UUnitStatComponent::GetAllFinalStats() const
{
	TMap<EUnitStatType, float> FinalStats;
	for (auto& BaseStat: BaseStats)
	{
		const float FinalStat = GetFinalStat(BaseStat.Key);
		FinalStats.Add(BaseStat.Key, FinalStat);
	}
	return FinalStats;
}

int32 UUnitStatComponent::AddModifiers(TArray<FUnitStatModifier> Modifiers)
{
	int32 NewIndex = ++LastModifierID;

	FUnitStatModifierStack Stack;
	Stack.Modifiers = Modifiers;
	ActiveModifiers.Add(NewIndex, Stack);

	//유닛 스탯 변경 이벤트 전달(UI 등)
	OnUnitStatChanged.Broadcast();

	return NewIndex;
}

int32 UUnitStatComponent::AddModifier(FUnitStatModifier Modifier)
{
	int32 NewIndex = ++LastModifierID;

	FUnitStatModifierStack Stack;
	Stack.Modifiers.Add(Modifier);
	ActiveModifiers.Add(NewIndex, Stack);

	//유닛 스탯 변경 이벤트 전달(UI 등)
	OnUnitStatChanged.Broadcast();

	return NewIndex;
}

bool UUnitStatComponent::RemoveModifiers(int32 Id)
{
	if (ActiveModifiers.Contains(Id))
	{
		ActiveModifiers.Remove(Id);
		OnUnitStatChanged.Broadcast();
		return true;
	}
	return false;
}

