// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DwarfUnderground/Unit/DwarfNameData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DwarfDataSubsystem.generated.h"

enum class EUnitState : uint8;
struct FUnitStateScript;
/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API UDwarfDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UDwarfDataSubsystem();

	UFUNCTION(BlueprintCallable)
	FDwarfNameData GetNextDwarfName();

	UFUNCTION(BlueprintPure)
	FUnitStateScript GetDwarfStateScript(EUnitState State) const;
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	//랜덤 순서 배열
	TArray<int32> RandomSequence;

	//현재 순서
	int32 CurrentIndex;

	//드워프 이름 데이터베이스
	UPROPERTY()
	UDataTable* NameTable;
	
	UPROPERTY()
	UDataTable* StateTable;
};
