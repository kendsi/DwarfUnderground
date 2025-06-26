// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ForgeSubsystem.generated.h"

struct FWeaponData;
/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API UForgeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	public:
	//전체 아이템 테이블
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UDataTable* ItemDataTable;
	
	//전체 무기 테이블
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UDataTable* WeaponDataTable;

public:
	UForgeSubsystem();
	
	//제작서 획득 시
	UFUNCTION(BlueprintCallable)
	void SetWeaponMakeable(FName WeaponName);

	UFUNCTION(BlueprintPure)
	bool GetWeaponMakeable(FName WeaponID) const;
	
	UFUNCTION(BlueprintCallable, Category = "Forge")
	FWeaponData GetWeaponData(FName ItemID) const;

	
	UFUNCTION(BlueprintCallable, Category = "Forge")
	TMap<FName, bool> GetAllWeaponData();

protected:
	
	void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	TMap<FName, bool> WeaponMakeableList;	
};
