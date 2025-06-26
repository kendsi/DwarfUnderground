// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentItem.h"

UEquipmentItem::UEquipmentItem()
{
	//테스트용 더미 클래스 로드
	// TODO: 아이템 체계 완성 후에는 각 아이템에 맞는 BP class 로드해야함
	FSoftClassPath ClassPath(TEXT("/Game/BluePrint/Equipment/BP_Pickaxe.BP_Pickaxe_C"));
	UClass* LoadedBPClass = ClassPath.TryLoadClass<AWeapon>();
    
	if (LoadedBPClass)
	{
		DummyEquipmentActor = LoadedBPClass;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentItem: 블루프린트 클래스 로드 실패"));
	}
}

bool UEquipmentItem::Equip(AUnit* TargetUnit)
{
	UE_LOG(LogTemp, Display, TEXT("EquipmentItem: 장비 장착 시도."));	
	if (EquippedUnit)
	{
		//해당 장비를 장비중인 유닛이 이미 존재
		UE_LOG(LogTemp, Warning, TEXT("EquipmentItem: 이 장비는 이미 다른 유닛에 장착중입니다."));
		return false;
	}

	// 액터 클래스 로드 확인
	if (!DummyEquipmentActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentItem: 장비 클래스가 설정되지 않았습니다"));
		return false;
	}
    
	// 월드 확인
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentItem: 유효한 월드가 없습니다"));
		return false;
	}
    
	// 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
	// 장비 액터 스폰
	AEquipmentBase* NewEquipment = World->SpawnActor<AWeapon>(
		DummyEquipmentActor,
		FVector(-2000.0, -2000.0, -2000.0),
		FRotator(0, 0, 0),
		SpawnParams
	);
    
	if (NewEquipment)
	{
		UE_LOG(LogTemp, Log, TEXT("EquipmentItem: 장비 생성 성공: %s"), *NewEquipment->GetName());
		TargetUnit->EquipItem(NewEquipment);
		EquippedUnit = TargetUnit;
		bIsVisible = false;
		OnItemStatusUpdated.Broadcast(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentItem: 장비 생성 실패"));
		return false;
	}

	return true;    
}

void UEquipmentItem::Unequip()
{
	OnItemStatusUpdated.Broadcast(this);
	if (!IsEquippable())
	{
		return;
	}
	// TODO: EquippedUnit->Unequip()
}

bool UEquipmentItem::IsEquippable()
{
	if (EquippedUnit) return false;
	else return true;
}
