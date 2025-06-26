// Fill out your copyright notice in the Description page of Project Settings.


#include "ForgeSubsystem.h"

#include "DwarfUnderground/Item/ItemData.h"
#include "DwarfUnderground/Item/WeaponData.h"

UForgeSubsystem::UForgeSubsystem()
{
	
	static ConstructorHelpers::FObjectFinder<UDataTable> ItemDataTableObj(
		TEXT("/Game/DataTable/ItemDataTable.ItemDataTable"));
	if (ItemDataTableObj.Succeeded())
	{
		ItemDataTable = ItemDataTableObj.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Forge: FATAL: ItemDataTable 로드 실패"));
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableObj(
		TEXT("/Game/DataTable/WeaponDataTable.WeaponDataTable"));
	if (WeaponDataTableObj.Succeeded())
	{
		WeaponDataTable = WeaponDataTableObj.Object;

		auto MyMap = WeaponDataTable->GetRowMap();
		for (auto Data : MyMap)
		{
			FWeaponData* Temp = WeaponDataTable->FindRow<FWeaponData>(Data.Key, TEXT("LookupWeaponData"));
			if (Temp)
			{
				WeaponMakeableList.Add(Data.Key, false);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Forge: 무기 전체 리스트 제작 중 일부 오류: %s가 없음"), *Data.Key.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Forge: FATAL: WeaponDataTable 로드 실패"));
	}
}

void UForgeSubsystem::SetWeaponMakeable(FName WeaponName)
{
	if (WeaponMakeableList.Contains(WeaponName))
	{
		WeaponMakeableList[WeaponName] = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Forge: WeaponMakeable 실패 ItemID: %s"), *WeaponName.ToString());		
	}
}

bool UForgeSubsystem::GetWeaponMakeable(FName WeaponID) const
{
	if (WeaponMakeableList.Contains(WeaponID))
	{
		return WeaponMakeableList[WeaponID];
	}
	return false;
}

FWeaponData UForgeSubsystem::GetWeaponData(FName ItemID) const
{
	if (WeaponDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Forge: WeaponDataTable이 NULL, ItemID: %s"), *ItemID.ToString());
		return FWeaponData();
	}

	FWeaponData* Data = WeaponDataTable->FindRow<FWeaponData>(ItemID, TEXT("LookupItemData"));
	if (Data == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Forge: 장비 데이터 없음, ItemID: %s"), *ItemID.ToString());
		return FWeaponData();
	}

	return FWeaponData(*Data);
}

void UForgeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	SetWeaponMakeable(FName("IronDagger"));
	SetWeaponMakeable(FName("Pickaxe"));
	SetWeaponMakeable(FName("Hammer"));
	SetWeaponMakeable(FName("SteelAxe"));
}

TMap<FName, bool> UForgeSubsystem::GetAllWeaponData()
{
	if (ItemDataTable == nullptr || WeaponDataTable == nullptr)
	{
		// TMap<FWeaponData, bool> ToReturn;
		// UE_LOG(LogTemp, Error, TEXT("Forge: DataTable이 NULL"));
		// for (auto Tuple: WeaponMakeableList)
		// {
		// 	FWeaponData WD = GetWeaponData(Tuple.Key);
		// 	ToReturn.Add(WD, Tuple.Value);
		// }
		UE_LOG(LogTemp, Error, TEXT("Forge: WeaponDataTable 로드 실패한듯"));
		return TMap<FName, bool>();

	}
		return WeaponMakeableList;
}
