// Fill out your copyright notice in the Description page of Project Settings.


#include "Forge.h"

#include "DwarfUnderground/Component/DwarfManagementComponent.h"
#include "DwarfUnderground/Component/UnitStatComponent.h"
#include "DwarfUnderground/Item/InventorySubSystem.h"
#include "DwarfUnderground/UI/LogUI.h"

bool AForge::TryManufactureItem(FName ItemID)
{
	//아이템 재료 유효한지 확인
	auto* Inventory = GetGameInstance()->GetSubsystem<UInventorySubSystem>();
	auto WeaponData = Inventory->GetWeaponData(ItemID);
	bool CanMake = true;
	for (auto Tuple : WeaponData.CraftResources)
	{
		if (Inventory->GetSingleItemNum(Tuple.Key)<Tuple.Value)
		{
			CanMake = false;
			break;
		}
	}
	if (!CanMake)
	{
		ULogUI::Log(TEXT("제작에 필요한 재료가 부족합니다."));
		return false;
	}
	
	auto AllDwarves = Management->GetDwarves();
	float MaxValue = -1.0f;
	ADwarfUnit* Worker = nullptr;
	for (auto Dwarf : AllDwarves)
	{
		if (ManufacturingProgresses.Contains(Dwarf)) continue;
		float Stat = Dwarf->Stats->GetFinalStat(EUnitStatType::Farming);
		if (Stat > MaxValue)
		{
			MaxValue = Stat;
			Worker = Dwarf;
		}
	}
	if (Worker != nullptr)
	{
		//작업 시작
		ManufacturingProgresses.Add(Worker, 0.0f);
		ManufacturingItems.Add(Worker, ItemID);
		Management->SignalDwarfUpdated();
		UE_LOG(LogTemp, Display, TEXT("Forge: 아이템 제작 시작: %s"), *ItemID.ToString());		
		
		//재료 소모
		for (auto Tuple : WeaponData.CraftResources)
		{
			Inventory->RemoveItem(Tuple.Key, Tuple.Value);
		}
		
		return true;
	}
	else
	{
		//작업 불가능
		//UILog 띄우기
		UE_LOG(LogTemp, Warning, TEXT("Forge: 드워프가 없어 아이템 제작 불가: %s"), *ItemID.ToString());
		FString ToLog = TEXT("제작을 위한 드워프가 부족합니다. ");
		if (AllDwarves.Num()==0) ToLog += TEXT("먼저 우측 UI에서 드워프를 할당하세요.");
		else ToLog += TEXT("모든 드워프가 다른 제작 중입니다.");
		ULogUI::Log(ToLog);
		return false;
	}
}

void AForge::BeginPlay()
{
	Super::BeginPlay();

	Management = GetComponentByClass<UDwarfManagementComponent>();
}

void AForge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//작업자 리스트 돌면서 작업자 상태 갱신
	//TODO: 아이템 따라 진행률 달라져야함?
	TArray<ADwarfUnit*> CompletedUnits;
	for (auto Iter = ManufacturingProgresses.CreateIterator(); Iter; ++Iter)
	{
		if (Iter->Value >= 1.0f)
		{
			CompletedUnits.Add(Iter->Key);
		}
		else
		{
			Iter->Value += DeltaTime * 0.3f;
		}
	}
	for (auto* Unit: CompletedUnits)
	{
		CompleteManufacturing(Unit);
	}
}

void AForge::CompleteManufacturing(const ADwarfUnit* TargetDwarf)
{
	FName ItemID;
	if (ManufacturingItems.Contains(TargetDwarf))
	{
		ItemID = ManufacturingItems[TargetDwarf];
		ManufacturingItems.Remove(TargetDwarf);
	}
	if (ManufacturingProgresses.Contains(TargetDwarf))
	{
		ManufacturingProgresses.Remove(TargetDwarf);
	}
	
	UE_LOG(LogTemp, Display, TEXT("Forge: 아이템 제작 완료: %s"), *ItemID.ToString());
	//여기에 아이템 제작
	if (auto* GI = GetGameInstance())
	{
		if (auto* Inventory = GI->GetSubsystem<UInventorySubSystem>())
		{
			Inventory->AddItem(ItemID);
		}
	}
}


float AForge::GetManufactureProgress(const ADwarfUnit* TargetDwarf) const
{
	if (ManufacturingProgresses.Contains(TargetDwarf))
	{
		return ManufacturingProgresses[TargetDwarf];
	}
	return -1.0f;
}


FName AForge::GetManufacturingItem(const ADwarfUnit* TargetDwarf) const
{
	if (ManufacturingItems.Contains(TargetDwarf))
	{
		return ManufacturingItems[TargetDwarf];
	}
	return FName("");
}


bool AForge::IsManufacturing(const ADwarfUnit* TargetDwarf) const
{
	if (ManufacturingProgresses.Contains(TargetDwarf))
	{
		return true;
	}
	return false;
}

void AForge::CancelManufacturing(const ADwarfUnit* TargetDwarf)
{
	if (ManufacturingItems.Contains(TargetDwarf))
	{
		//재료 원복
		FName Weapon = ManufacturingItems[TargetDwarf];
		auto* Inventory = GetGameInstance()->GetSubsystem<UInventorySubSystem>();
		auto WeaponData = Inventory->GetWeaponData(Weapon);
		
		for (auto Tuple : WeaponData.CraftResources)
		{
			Inventory->AddItem(Tuple.Key, Tuple.Value);
		}
		
		ManufacturingItems.Remove(TargetDwarf);
	}
	if (ManufacturingProgresses.Contains(TargetDwarf))
	{
		ManufacturingProgresses.Remove(TargetDwarf);
	}
	
}
