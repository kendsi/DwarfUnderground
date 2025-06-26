// Copyright Epic Games, Inc. All Rights Reserved.

#include "DwarfUndergroundGameMode.h"
#include "DwarfUndergroundPlayerController.h"
#include "DwarfUndergroundCharacter.h"
#include "UI/SelectionHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Unit/CameraPawn.h"

ADwarfUndergroundGameMode::ADwarfUndergroundGameMode()
{
	HUDClass = ASelectionHUD::StaticClass();
	PlayerControllerClass = ADwarfUndergroundPlayerController::StaticClass();
	DefaultPawnClass = ACameraPawn::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BluePrint/Unit/BP_CameraPawn"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/BluePrint/BP_PlayerController"));
	if (PlayerControllerBPClass.Class != nullptr)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}