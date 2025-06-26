#pragma once
#include "CoreMinimal.h"
#include "ItemType.generated.h"


// TODO: uint8로는 부족할 수 있음.(max: 256)
// TODO: 데이터 테이블(csv, json or Unreal Data Table)과 연동하는 것이 더 깔끔해보임
UENUM(BlueprintType)
enum class EItemType : uint8
{
	NONE = 0,
	WEAPON = 1,	// To Weapon
	EQUIP = 2,
	COMPSUMTION = 3,
	RESOURCE = 4,
	RECIPE = 5,
	TOOL = 6,
	SHIELD = 7,
};