// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadCreatePanel.h"

#include "DwarfUndergroundHUD.h"
#include "LogUI.h"
#include "DwarfUnderground/Unit/DwarfManagerSubsystem.h"
#include "DwarfUnderground/Unit/Squad/SquadComponent.h"

USquadCreatePanel::USquadCreatePanel(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

void USquadCreatePanel::NativeConstruct()
{
	Super::NativeConstruct();

	//PlayerController 획득
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PlayerController = Cast<ADwarfUndergroundPlayerController>(PC);
	}
}

bool USquadCreatePanel::CreateSquad()
{
	auto* DwarfManager = GetGameInstance()->GetSubsystem<UDwarfManagerSubsystem>();
	//TODO: 관리자가 필요할 것 같다. 지금은 급한대로 UI에서 진행
	if (TemporarySquad.Num()<DwarfManager->MinPersonnel)
	{
		UE_LOG(LogTemp, Warning, TEXT("부대 생성 실패, 최소 인원 수 미충족"));
		ULogUI::Log(TEXT("부대 생성 실패, 최소 인원 수 미충족"));
		return false;
	}
	if (TemporarySquad.Num()>DwarfManager->MaxPersonnel)
	{
		UE_LOG(LogTemp, Warning, TEXT("부대 생성 실패, 최대 인원 수보다 사람이 많음"));
		ULogUI::Log(TEXT("부대 생성 실패, 최대 인원 수보다 사람이 많음"));
		return false;
	}

	USquadComponent* CreatedSquad = PlayerController->CreateEmptySquad();
	if (!CreatedSquad) {
		return false;
	}

	for (auto* Unit : TemporarySquad)
	{
		PlayerController->AddUnitToSquad(CreatedSquad->GetSquadNumber(), Unit);
	}

	auto* Manager = GetGameInstance()->GetSubsystem<UDwarfManagerSubsystem>();
	Manager->RegisterSquad(CreatedSquad);
	return true;
}

void USquadCreatePanel::UpdateSquad(USquadComponent* Squad)
{
	TArray<AUnit*> OrgUnits = Squad->Units;
	TArray<AUnit*> NewUnits;
	for (auto* TempUnit: TemporarySquad)
	{
		if (OrgUnits.Contains(TempUnit))
		{
			OrgUnits.Remove(TempUnit);
		}
		else
		{
			NewUnits.Add(TempUnit);
		}
	}

	for (auto* OrgUnit: OrgUnits)
	{
		Squad->RemoveUnit(OrgUnit);
	}
	for (auto* NewUnit: NewUnits)
	{
		Squad->AddUnit(NewUnit);
		//다른 유닛들은 이미 선택되어있었으므로 새로 추가된애들을 선택함.
		NewUnit->OnSelected();
		UE_LOG(LogTemp, Warning, TEXT("유닛 추가됨"));
	}
	return;
}
