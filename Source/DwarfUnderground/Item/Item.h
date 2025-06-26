// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "ItemType.h"
#include "Item.generated.h"

/**
 * 
 */

//아이템 상태 변경시에 대한 Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemStatusUpdatedDelegate, UItem*, UpdatedItem);

UCLASS()
class DWARFUNDERGROUND_API UItem : public UObject
{
	GENERATED_BODY()

public:
	//아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;
	
	//아이템 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDescription;

	//아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon;

	//스택 가능 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bIsStackable;

	//(스택 가능한 경우) 아이템 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Quantity;

	//아이템 상태 변화 시 호출되는 이벤트
	// TODO: Setter 등 호출 시에도 변화되어야
	FOnItemStatusUpdatedDelegate OnItemStatusUpdated;

public:
	UItem();

	//Get ID
	uint32 GetID() const;
	bool IsVisible() const;

protected:
	static uint32 GetNextItemID();

	uint32 ID;
	bool bIsVisible;  //장비 장착중 등 인벤토리에서 보이면 안되는 경우
	EItemType ItemType; //아이템 타입 - 추후 데이터 테이블에서 불러와야함 
};