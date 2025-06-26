#pragma once

#include "CoreMinimal.h"

//주의: DisplayName과 StateScriptData의 Name(ROW)이 같아야함
UENUM(BlueprintType)
enum class EUnitState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Gathering       UMETA(DisplayName = "Gathering"),
	Working			UMETA(DisplayName = "Working"),
	InSquad			UMETA(DisplayName = "InSquad"),
	Combat  		UMETA(DisplayName = "Combat"),
	SoilDigging			UMETA(DisplayName = "SoilDigging"),
	SoilCovering			UMETA(DisplayName = "SoilCovering"),
	EquipCrafting	UMETA(DisplayName = "EquipCrafting"),
	Refining			UMETA(DisplayName = "Refining"),
	Recruiting			UMETA(DisplayName = "Recruiting"),
	Farming			UMETA(DisplayName = "Farming"),
	Researching			UMETA(DisplayName = "Researching"),
};