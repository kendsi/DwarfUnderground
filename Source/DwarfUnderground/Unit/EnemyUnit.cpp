// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyUnit.h"
#include "DwarfUnderground/AI/Controller/EnemyAIController.h"
#include "DwarfUnderground/Equipment/EquipmentBase.h"
#include "GameFramework/CharacterMovementComponent.h"

int32 AEnemyUnit::AliveCount = 0;
FOnAllEnemiesDead AEnemyUnit::OnAllEnemiesDead;

AEnemyUnit::AEnemyUnit()
{
    EquippedWeapon = nullptr;
	EquippedArmor = nullptr;
	EquippedShield = nullptr;
}

void AEnemyUnit::BeginPlay() {
    Super::BeginPlay();

    ++AliveCount;

    if (!EquippedWeapon && DefaultWeaponClass)
    {
        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.Instigator = GetInstigator();
        Params.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // 인스턴스 생성
        EquippedWeapon = GetWorld()->SpawnActor<AEquipmentBase>(
            DefaultWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

        // 메쉬 소켓에 부착
        EquipItem(EquippedWeapon);
    }
    if (!EquippedShield && DefaultShieldClass) {
        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.Instigator = GetInstigator();
        Params.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // 인스턴스 생성
        EquippedShield = GetWorld()->SpawnActor<AEquipmentBase>(
            DefaultShieldClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

        // 메쉬 소켓에 부착
        EquipItem(EquippedShield);
    }
}

void AEnemyUnit::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    AliveCount--;
    if (AliveCount <= 0)
    {
        OnAllEnemiesDead.Broadcast();   // 모두 사망 → 신호
    }
    Super::EndPlay(EndPlayReason);
}