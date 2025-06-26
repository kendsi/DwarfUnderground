// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySubSystem.h"
#include "WeaponData.h"
#include "DwarfUnderground/BuildManagement/BuildingPriceData.h"
#include "DwarfUnderground/BuildManagement/Buildings/BuildingBase.h"
#include "DwarfUnderground/BuildManagement/MapBase/Soil.h"

UInventorySubSystem::UInventorySubSystem()
{
	MaxSlots = 32;
	
	static ConstructorHelpers::FObjectFinder<UDataTable> ItemDataTableObj(TEXT("/Game/DataTable/ItemDataTable.ItemDataTable"));
	if (ItemDataTableObj.Succeeded())
	{
		ItemDataTable = ItemDataTableObj.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory: FATAL: ItemDataTable 로드 실패"));
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableObj(TEXT("/Game/DataTable/WeaponDataTable.WeaponDataTable"));
	if (WeaponDataTableObj.Succeeded())
	{
		WeaponDataTable = WeaponDataTableObj.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory: FATAL: WeaponDataTable 로드 실패"));
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> BuildingPriceTableObj(TEXT("/Game/DataTable/BuildingPriceDataTable.BuildingPriceDataTable"));
	if (BuildingPriceTableObj.Succeeded())
	{
		BuildingPriceTable = BuildingPriceTableObj.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory: FATAL: BuildingPriceTable 로드 실패"));
	}
}

void UInventorySubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	CreateDummyItems();
}

TMap<FName, int32> UInventorySubSystem::GetAllItems()
{
	return InventoryItems;
}

TMap<FName, int32> UInventorySubSystem::GetVisibleItems()
{
	UE_LOG(LogTemp, Warning, TEXT("Inventory.GetVisibleItems: Deprecated됨"));
	return InventoryItems;
}

void UInventorySubSystem::AddItem(FName Item, int32 Amount)
{
	//유효성 검사
	FItemData Tester = GetItemData(Item);
	if (Tester.ItemID == FName(""))
	{
		UE_LOG(LogTemp, Warning, TEXT("아이템 추가 실패, 유효하지 않은 아이템: %s"),*Item.ToString());
		return;
	}

	//아이템 추가
	if (!InventoryItems.Contains(Item))
	{
		InventoryItems.Add(Item, Amount);
	}
	else
	{
		InventoryItems[Item] += Amount;
	}
	OnInventoryUpdated.Broadcast(Item, Amount);
}

bool UInventorySubSystem::RemoveItem(FName Item, int32 Amount)
{
	if (!InventoryItems.Contains(Item))
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory: 아이템이 존재하지 않음: %s"), *Item.ToString());
		return false;
	}
	if (InventoryItems[Item] > Amount)
	{
		InventoryItems[Item] -= Amount;
		OnInventoryUpdated.Broadcast(Item, -Amount);
		return true;
	}
	if (InventoryItems[Item] == Amount)
	{
		InventoryItems.Remove(Item);
		OnInventoryUpdated.Broadcast(Item, -Amount);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory: 지우려는 아이템 수보다 아이템이 적음"));
		return false;
	}
}

int32 UInventorySubSystem::GetSingleItemNum(FName ItemID) const
{
	if (!InventoryItems.Contains(ItemID))
	{
		return 0;
	}
	return InventoryItems[ItemID];
}

const FItemData UInventorySubSystem::GetItemData(FName ItemID) const
{
	if (ItemDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory: DataTable이 NULL, ItemID: %s"),*ItemID.ToString());
		return FItemData();
	}

	FItemData* Data = ItemDataTable->FindRow<FItemData>(ItemID, TEXT("LookupItemData"));
	if (Data == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory: 아이템을 찾을 수 없음, ItemID: %s"),*ItemID.ToString());
		return FItemData();
	}
	
	return FItemData(*Data);
}

const FWeaponData UInventorySubSystem::GetWeaponData(FName ItemID) const
{
	if (WeaponDataTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory: WeaponDataTable이 NULL, ItemID: %s"),*ItemID.ToString());
		return FWeaponData();
	}

	FWeaponData* Data = WeaponDataTable->FindRow<FWeaponData>(ItemID, TEXT("LookupItemData"));
	if (Data == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Inventory: 장비 데이터 없음, ItemID: %s"),*ItemID.ToString());
		return FWeaponData();
	}
	
	return FWeaponData(*Data);
}

int32 UInventorySubSystem::GetItemNum() const
{
	return InventoryItems.Num();
}

TMap<FName, int32> UInventorySubSystem::GetBuildResourceByActor(TSubclassOf<AActor> ToBuild) const
{	
	UObject* CDO = ToBuild->GetDefaultObject();
	if (CDO == nullptr) return TMap<FName, int32>();

	if (ABuildingBase* Building = Cast<ABuildingBase>(CDO))
	{
		//빌딩일 경우	
		FBuildingPriceData Data(*BuildingPriceTable->FindRow<FBuildingPriceData>(Building->BuildingID,TEXT("LookupItemData")));
		return Data.RequiredItem;
	}
	else if (ASoil* Soil = Cast<ASoil>(CDO))
	{
		//빌딩일 경우	
		FBuildingPriceData Data(*BuildingPriceTable->FindRow<FBuildingPriceData>(FName(TEXT("Soil")),TEXT("LookupItemData")));
		return Data.RequiredItem;
	}
	else
	{
		//잘못된 액터
		return TMap<FName, int32>();
	}
}

bool UInventorySubSystem::CheckEnoughPriceToBuild(TSubclassOf<AActor> BuildType)
{
	auto RequiredItems = GetBuildResourceByActor(BuildType);
	
	if (RequiredItems.Num()==0) return false;	//잘못된 데이터 읽음
	
	for (auto Item : RequiredItems)
	{
		if (InventoryItems.Contains(Item.Key))
		{
			if (InventoryItems[Item.Key]<Item.Value)
			{
				//아이템을 적게 가지고 있음
				return false;
			}
		}
		else
		{
			//해당 아이템이없음
			return false;
		}
	}
	return true;	
}

bool UInventorySubSystem::ConsumeBuildResource(TSubclassOf<AActor> BuildType)
{
	auto RequiredItems = GetBuildResourceByActor(BuildType);
		
	for (auto Item : RequiredItems)
	{
		if (!RemoveItem(Item.Key, Item.Value)) return false;
	}
	return true;	
}

void UInventorySubSystem::CreateDummyItems()
{
	FItemData Item1 = GetItemData(FName("Pickaxe"));
	FItemData Item2 = GetItemData(FName("OldRecipe"));
	FItemData Item3 = GetItemData(FName("GoldIngot"));
	FItemData Item4 = GetItemData(FName("IronMace"));
	FItemData Item5 = GetItemData(FName("IronDagger"));
	FItemData Item6 = GetItemData(FName("Hammer"));
	FItemData Item7 = GetItemData(FName("SteelShield"));
	FItemData Item8 = GetItemData(FName("Wood"));
	FItemData Item9 = GetItemData(FName("Dirt"));
	FItemData Item10 = GetItemData(FName("IronIngot"));
	FItemData Item11 = GetItemData(FName("Stone"));
	FItemData Item12 = GetItemData(FName("GoldOre"));
	FItemData Item13 = GetItemData(FName("IronOre"));
	AddItem(Item1.ItemID,12);
	AddItem(Item2.ItemID);
	AddItem(Item3.ItemID, 20);
	AddItem(Item4.ItemID, 4);
	AddItem(Item5.ItemID, 5);
	AddItem(Item6.ItemID, 30);
	AddItem(Item7.ItemID, 20);
	AddItem(Item8.ItemID, 30);
	AddItem(Item9.ItemID, 20);
	AddItem(Item10.ItemID, 20);
	AddItem(Item11.ItemID, 20);
	AddItem(Item12.ItemID, 40);
	AddItem(Item13.ItemID, 40);
}
