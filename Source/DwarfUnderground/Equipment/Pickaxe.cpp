// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickaxe.h"
#include "DwarfUnderground/Resource/ResourceBase.h"

// Sets default values
APickaxe::APickaxe()
{
	AttackPower = 10.0f;
	AttackRange = 150.0f;
	AttackCooldown = 2.0f;
	Accuracy = -10.0f;
	EquipmentName = "Pickaxe";
}

int32 APickaxe::MineResource(AResourceBase* Resource, int32 Amount)
{
	if (!Resource)
	{
		return false;
	}

	int32 ResourceAmount = 0;
	if (Resource)
	{
		ResourceAmount = Resource->OnCollected(Amount);
	}

	return ResourceAmount;
}

