// Fill out your copyright notice in the Description page of Project Settings.


#include "LogUI.h"

#include "DwarfUndergroundHUD.h"

void ULogUI::Log(FString Text, float Duration)
{
	
	auto* HUD = UDwarfUndergroundHUD::GetInstance();
	if (!HUD) return;

	FLinearColor Color;
	Color.R=1.0f;
	Color.G=1.0f;
	Color.B=.8f;
	Color.A=1.0f;
	HUD->UILog(Text, Duration, Color);
}
