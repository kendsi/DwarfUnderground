// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{
	EquipmentName = "Default Weapon";
	AttackPower = 0.0f;
	AttackRange = 0.0f;
	AttackCooldown = 0.0f;
	Accuracy = 10.0f;
}

void AWeapon::AttackEnemy(AActor* Enemy)
{
}