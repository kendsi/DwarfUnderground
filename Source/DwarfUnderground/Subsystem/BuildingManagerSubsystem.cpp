// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingManagerSubsystem.h"

void UBuildingManagerSubsystem::RegisterBuilding(ABuildingBase* Building)
{
	Buildings.Add(Building);
	OnBuildingConstructed.Broadcast(Building);
	OnBuildingListUpdated.Broadcast(Buildings);
}

void UBuildingManagerSubsystem::UnregisterBuilding(ABuildingBase* Building)
{
	Buildings.Remove(Building);
	OnBuildingDestructed.Broadcast(Building);
	OnBuildingListUpdated.Broadcast(Buildings);
}

TArray<ABuildingBase*> UBuildingManagerSubsystem::GetAllBuildings()
{
	return Buildings;
}
