// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfUnit.h"

#include "DwarfManagerSubsystem.h"
#include "DwarfUnderground/Equipment/EquipmentBase.h"
#include "DwarfUnderground/Equipment/Weapon.h"
#include "DwarfUnderground/Item/InventorySubSystem.h"
#include "DwarfUnderground/Resource/ResourceBase.h"

// Sets default values
ADwarfUnit::ADwarfUnit()
{
	EquippedWeapon = nullptr;
	EquippedTool = nullptr;
	EquippedArmor = nullptr;
	EquippedShield = nullptr;
	DwarfName = TEXT("");
}

bool ADwarfUnit::EquipItemFromInventory(FItemData Item)
{
	//TODO: 무기 아닌 장비도 장착 가능해야함
	//인벤토리
	auto* Inventory = GetGameInstance()->GetSubsystem<UInventorySubSystem>();
	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("아이템 장착 실패: 인벤토리가 없습니다"));
		return false;
	}
	//장착 불가능하면 return
	if (Inventory->GetItemNum() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("아이템 장착 실패: 아이템이 부족합니다."));
		return false;
	}

	//장비 아이템인지 확인&해당 아이템 클래스 불러오기
	auto ValidData = Inventory->GetItemData(Item.ItemID);
	if ((ValidData.Type != EItemType::WEAPON && ValidData.Type != EItemType::EQUIP && ValidData.Type != EItemType::SHIELD&&ValidData.Type!=EItemType::TOOL))
	{
		UE_LOG(LogTemp, Error, TEXT("아이템 장착 실패: 대상 아이템이 무기나 장비가 아닙니다."));
		return false;
	}
	auto WeaponData = Inventory->GetWeaponData(Item.ItemID);
	if (WeaponData.ItemID.IsEqual(FName("")))
	{
		UE_LOG(LogTemp, Error, TEXT("아이템 장착 실패: 대장 장비가 장비 테이블에 없습니다."));
		return false;
	}

	//부모에게 장착
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 장비 액터 스폰
	AEquipmentBase* NewEquipment = GetWorld()->SpawnActor<AEquipmentBase>(
		WeaponData.EquipmentBaseClass,
		FVector(-2000.0, -2000.0, -2000.0),
		FRotator(0, 0, 0),
		SpawnParams
	);
	FName LastWeapon = EquippedWeapon==nullptr?FName(""):EquippedWeapon->ItemID;
	FName LastArmor = EquippedArmor==nullptr?FName(""):EquippedArmor->ItemID;
	FName LastShield = EquippedShield==nullptr?FName(""):EquippedShield->ItemID;
	FName LastTool = EquippedTool==nullptr?FName(""):EquippedTool->ItemID;
	if (NewEquipment)
	{
		//무기, 도구, 방패에 따라 다르게 장착
		Super::EquipItem(NewEquipment);		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("아이템 장착 실패: 액터 생성 실패"));
		return false;
	}

	//장착 직전 인벤토리에서 아이템 소모
	//TODO: 부모의 EquipItem 성공여부
	if (!Inventory->RemoveItem(Item.ItemID, 1))
	{
		UE_LOG(LogTemp, Error, TEXT("아이템 장착 실패: 아이템이 부족합니다. <FATAL>"));
		return false;
	}
	//가지고 있던 장비는 되돌림
	if (ValidData.Type == EItemType::WEAPON)
	{		
		if (LastWeapon != TEXT(""))
			Inventory->AddItem(LastWeapon, 1);
		Stats->RemoveModifiers(LastEquippedWeaponStatModifierID);
		LastEquippedWeaponStatModifierID = Stats->AddModifiers(Inventory->GetWeaponData(Item.ItemID).StatModifiers);
	}
	if (ValidData.Type == EItemType::TOOL)
	{
		if (LastTool != TEXT(""))
			Inventory->AddItem(LastTool, 1);
		Stats->RemoveModifiers(LastEquippedToolStatModifierID);
		LastEquippedToolStatModifierID = Stats->AddModifiers(Inventory->GetWeaponData(Item.ItemID).StatModifiers);
	}
	if (ValidData.Type == EItemType::SHIELD)
	{		
		if (LastShield != TEXT(""))
		Inventory->AddItem(LastShield, 1);
		Stats->RemoveModifiers(LastEquippedShieldStatModifierID);
		LastEquippedShieldStatModifierID = Stats->AddModifiers(Inventory->GetWeaponData(Item.ItemID).StatModifiers);
	}
	//TODO: ARMOR


	//장착 성공 시 능력치 반영

	LastEquippedWeapon = Item;
	return true;
}


// Called when the game starts or when spawned
void ADwarfUnit::BeginPlay()
{
	Super::BeginPlay();

	//드워프 생성 시 드워프 관리자에 등록
	if (auto* GameInstance = GetGameInstance())
	{
		if (auto* DwarfManager = GameInstance->GetSubsystem<UDwarfManagerSubsystem>())
		{
			//모병소에서 생성된 경우, Deferred Spawn으로 이미 등록되어있을 수 있음.
			if (!DwarfManager->CheckIsRegistered(this))
			{
				DwarfManager->RegisterDwarf(this);
			}
		}
	}

	//내 드워프 아이콘 등록
	if (DwarfIconList.Num()>0)
	{
		int32 RandInt = FMath::RandRange(0, DwarfIconList.Num()-1);
		DwarfIcon = DwarfIconList[RandInt];		
	}
	
}

void ADwarfUnit::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	//드워프 삭제 시 드워프 관리자에 등록
	if (auto* GameInstance = GetGameInstance())
	{
		if (auto* DwarfManager = GameInstance->GetSubsystem<UDwarfManagerSubsystem>())
		{
			DwarfManager->UnregisterDwarf(this);
		}
	}
}

// Called every frame
void ADwarfUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
