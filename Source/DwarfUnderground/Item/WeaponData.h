#pragma once
#include "../Component/UnitStatComponent.h"
#include "WeaponData.generated.h"

class AEquipmentBase;

USTRUCT(BlueprintType)
struct DWARFUNDERGROUND_API FWeaponData: public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemID;

	//드워프가 장착하는 아이템으로
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "드워프가 실제로 장착하는 블루프린트 클래스"))
	TSubclassOf<AEquipmentBase> EquipmentBaseClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponType;

	//DEPRECATED
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Power;

	//장착 시 상승하는 능력치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FUnitStatModifier> StatModifiers;

	//제작시 필요한 것들
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> CraftResources;
};