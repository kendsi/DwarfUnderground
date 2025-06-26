// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfDataSubsystem.h"
#include "Algo/RandomShuffle.h"
#include "DwarfUnderground/Unit/UnitState.h"
#include "../Unit/UnitStateScript.h"

UDwarfDataSubsystem::UDwarfDataSubsystem()
{
	CurrentIndex = 0;
	static ConstructorHelpers::FObjectFinder<UDataTable> NameTableFinder(TEXT("/Game/DataTable/DwarfNameDataTable.DwarfNameDataTable"));
	if (NameTableFinder.Succeeded())
	{
		NameTable = NameTableFinder.Object;
	}

	//UnitStateScript Load
	static ConstructorHelpers::FObjectFinder<UDataTable> StateTableFinder(TEXT("/Game/DataTable/DwarfStateScript.DwarfStateScript"));
	if (StateTableFinder.Succeeded())
	{
		StateTable = StateTableFinder.Object;
	}
}

FDwarfNameData UDwarfDataSubsystem::GetNextDwarfName()
{
	FName Index = FName(*FString::FromInt(RandomSequence[CurrentIndex]));
	
	FDwarfNameData* Data = NameTable->FindRow<FDwarfNameData>(Index, TEXT("LookupItemData"));
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("DwarfDataSubsystem: 발생해선 안되는 오류: 드워프이름 Index가 존재하지 않음"));
		return FDwarfNameData();
	}

	CurrentIndex++;
	if (CurrentIndex==RandomSequence.Num())
	{
		CurrentIndex = 0;
	}

	return FDwarfNameData(*Data);
}

FUnitStateScript UDwarfDataSubsystem::GetDwarfStateScript(EUnitState State) const
{
	FName Index = FName((*UEnum::GetDisplayValueAsText(State).ToString()));
	
	FUnitStateScript* Data = StateTable->FindRow<FUnitStateScript>(Index, TEXT("LookupItemData"));
	if (!Data)
	{
		UE_LOG(LogTemp, Error, TEXT("DwarfDataSubsystem: 발생해선 안되는 오류: 드워프 State Index가 존재하지 않음"));
		return FUnitStateScript();
	}

	return FUnitStateScript(*Data);
}

void UDwarfDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!NameTable)
	{
		UE_LOG(LogTemp, Error, TEXT("DwarfDataSubSystem: 드워프 이름 테이블이 존재하지않음!!!"));
		return;
	}

	RandomSequence.Empty();
	for (int32 i = 1; i <= NameTable->GetRowMap().Num(); ++i)
	{
		RandomSequence.Add(i);
	}

	// 숫자 섞기
	Algo::RandomShuffle(RandomSequence);	
}
