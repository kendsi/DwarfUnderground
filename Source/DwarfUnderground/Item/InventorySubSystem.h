// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "ItemData.h"
#include "WeaponData.h"
#include "UObject/NoExportTypes.h"
#include "InventorySubSystem.generated.h"


//Design
// Inventory: 실제 아이템들과 유저와의 Interface 역할
// 현재 인벤토리는 모든 상황에서 공유하므로, PlayerController에 존재할 예정
//  - 추후 변경 가능성

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryUpdatedDelegate, FName, ItemID, int32, Amount);

UCLASS()
class DWARFUNDERGROUND_API UInventorySubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	//인벤토리 업데이트 시 호출될 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdatedDelegate OnInventoryUpdated;

	//인벤토리 슬롯 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 MaxSlots;

	//전체 아이템 테이블
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UDataTable* ItemDataTable;
	
	//전체 무기 테이블
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UDataTable* WeaponDataTable;
	
	//빌딩 가격 테이블
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UDataTable* BuildingPriceTable;
	
public:
	UInventorySubSystem();

	void Initialize(FSubsystemCollectionBase& Collection) override;
	
	//아이템 TArray 형태로 반환
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TMap<FName, int32> GetAllItems(); //모든 아이템
	TMap<FName, int32> GetVisibleItems(); //장착|사용 가능한 아이템만
  
	// 아이템 추가
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(FName ItemID, int32 Amount=1);

	//아이템 삭제
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemID, int32 Amount=1);

	//아이템 개수
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Inventory")
	int32 GetSingleItemNum(FName ItemID) const;

	//아이템 데이터 가져옴
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const FItemData GetItemData(FName ItemID) const;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const FWeaponData GetWeaponData(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemNum() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	TMap<FName, int32> GetBuildResourceByActor(TSubclassOf<AActor> ToBuild) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CheckEnoughPriceToBuild(TSubclassOf<AActor> BuildType);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeBuildResource(TSubclassOf<AActor> BuildType);


private:
	// 실제 가지고 있는 아이템 리스트 <ID, 아이템>
	TMap<FName, int32> InventoryItems; //ID, 아이템	

	//더미 아이템 생성
	void CreateDummyItems();
};