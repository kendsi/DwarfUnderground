// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DwarfUnderground/Interface/SelectableInterface.h"
#include "SquadProxy.generated.h"

class USquadComponent;

UCLASS()
class DWARFUNDERGROUND_API ASquadProxy : public AActor, public ISelectableInterface
{
	GENERATED_BODY()
	
public:
	ASquadProxy();

	UPROPERTY(BlueprintReadWrite)
	USquadComponent* OwnerSquad = nullptr;

	UPROPERTY()
	USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsStatic;

	virtual void Tick(float) override;

	// ISelectable
	UFUNCTION(BlueprintCallable)
	virtual void OnSelected() override;

	UFUNCTION(BlueprintCallable)
	virtual void OnDeselected() override;
	virtual FVector GetSelectPoint() const override;
};
