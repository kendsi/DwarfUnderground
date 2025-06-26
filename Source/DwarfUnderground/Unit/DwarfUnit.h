// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unit.h"
#include "DwarfUnderground/Item/ItemData.h"
#include "DwarfUnderground/Item/WeaponData.h"
#include "DwarfUnit.generated.h"

UCLASS()
class DWARFUNDERGROUND_API ADwarfUnit : public AUnit
{
	GENERATED_BODY()

public:
	ADwarfUnit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dwarf")
	FString DwarfName;

	//TODO: 이런 식으로 무기가 저장되는 것은 위험하다... 부모의 Equip과 합칠 수 있도록
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dwarf")
	FItemData LastEquippedWeapon;

	//TODO: 부모의 무기와 통합 시 삭제
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dwarf")
	FName ToolID;

	//마지막으로 장착한 무기 능력치 정보
	int32 LastEquippedWeaponStatModifierID;
	int32 LastEquippedShieldStatModifierID;
	int32 LastEquippedToolStatModifierID;

	//06.24: 임시로 제작
	//드워프 이미지 리스트, 이중에서 랜덤으로 선택
	UPROPERTY(EditDefaultsOnly, Category = "Dwarf", meta = (AllowPrivateAccess = "true"))
	TArray<UTexture2D*> DwarfIconList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dwarf")
	UTexture2D* DwarfIcon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 인벤토리에서 장비를 찾아 장착합니다.
	UFUNCTION(BlueprintCallable)
	bool EquipItemFromInventory(FItemData Item);

};
