// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfDetailsPanel.h"

void UDwarfDetailsPanel::OpenPanel(ADwarfUnit* Target)
{
	SetVisibility(ESlateVisibility::Visible);
	UpdatePanel(Target);
}

void UDwarfDetailsPanel::ClosePanel()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UDwarfDetailsPanel::UpdatePanel(ADwarfUnit* Target)
{
    OnUpdateDetailsPanel.Broadcast(Target);

	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("DetailsPanel: Update 실패, 타겟 드워프가 존재하지 않음"));
		return;
	}

	// DwarfNameTextBlock->SetText(FText::FromString(CurrentStatus.Name));
	//
	// HPTextBlock->SetText(FText::AsNumber(CurrentStatus.HP));
	//
	// OffenseTextBlock->SetText(FText::AsNumber(CurrentStatus.AttackPower));
	//
	// DefenseTextBlock->SetText(FText::AsNumber(CurrentStatus.DefensePower));
	//
	// AccuracyTextBlock->SetText(FText::AsNumber(CurrentStatus.AccuracyRate));
	//
	// DodgeTextBlock->SetText(FText::AsNumber(CurrentStatus.DodgeRate));
	//
	// MiningTextBlock->SetText(FText::AsNumber(CurrentStatus.MiningAbility));
	//
	// BuildingTextBlock->SetText(FText::AsNumber(CurrentStatus.BuildingAbility));
	//
	// FarmingTextBlock->SetText(FText::AsNumber(CurrentStatus.FarmingAbility));
	//
	// RecruitingTextBlock->SetText(FText::AsNumber(CurrentStatus.RecruitingAbility));

	//인벤토리를 업데이트
	//InventoryPanel->UpdatePanel(Target);
}
