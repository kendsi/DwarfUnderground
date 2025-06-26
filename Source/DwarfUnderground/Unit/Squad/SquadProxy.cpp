// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadProxy.h"
#include "SquadComponent.h"
#include "DwarfUnderground/Unit/Unit.h"

ASquadProxy::ASquadProxy()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	SetActorEnableCollision(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	bIsStatic = false;
}

void ASquadProxy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if ((!OwnerSquad || OwnerSquad->Units.Num() == 0) && !bIsStatic)
	{
		Destroy();
		return;
	}
	SetActorLocation(OwnerSquad->Formation.Anchor);
}

void ASquadProxy::OnSelected() {
	if (OwnerSquad) {
		TArray<AUnit*> Units = OwnerSquad->Units;
		for (int i = 0; i < Units.Num(); i++) {
			if (IsValid(Units[i]))
				Units[i]->OnSelected();
		}
	}
}
void ASquadProxy::OnDeselected() {
	if (OwnerSquad) {
		TArray<AUnit*> Units = OwnerSquad->Units;
		for (int i = 0; i < Units.Num(); i++) {
			if (IsValid(Units[i]))
				Units[i]->OnDeselected();
		}
	}
}
FVector ASquadProxy::GetSelectPoint() const { return GetActorLocation(); }