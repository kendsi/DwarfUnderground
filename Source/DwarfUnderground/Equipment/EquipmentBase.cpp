// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentBase.h"

#include "DwarfUnderground/Item/InventorySubSystem.h"

// Sets default values
AEquipmentBase::AEquipmentBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEquipmentBase::BeginPlay()
{
	Super::BeginPlay();

	//장비 데이터 로드
	auto* GI = GetGameInstance();
	if (!GI) return;
	auto* Inventory = GI->GetSubsystem<UInventorySubSystem>();
	if (!Inventory) return;

	auto ItemData = Inventory->GetWeaponData(ItemID);
	if (ItemData.ItemID == FName())
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentBase: 데이터 로드 실패, Item ID가 잘못되었습니다."));
		return;
	}

	StatModifiers = ItemData.StatModifiers;
	
}

void AEquipmentBase::OnEquipped(AUnit* Unit)
{
	UE_LOG(LogTemp, Log, TEXT("%s equipped by Unit"), *EquipmentName.ToString());
}

void AEquipmentBase::OnUnequipped(AUnit* Unit)
{
	UE_LOG(LogTemp, Log, TEXT("%s unequipped by Unit"), *EquipmentName.ToString());
}