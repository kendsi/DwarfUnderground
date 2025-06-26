// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DwarfUnderground/Unit/Squad/FormationData.h"
#include "SelectionHUD.generated.h"

class USquadComponent;
class UUnitInteractionComponent;

USTRUCT()
struct FFormationPreview
{
    GENERATED_BODY()

    UPROPERTY() TArray<FFormationData> Forms;
    UPROPERTY() TArray<USquadComponent*> Squads;
    UPROPERTY() float Alpha = 0.7f;   // 투명도
    UPROPERTY() bool  bFinal = false;  // 드래그 종료 후인가
};

UCLASS()
class DWARFUNDERGROUND_API ASelectionHUD : public AHUD
{
	GENERATED_BODY()

public:
    // 드래그 중인지 여부, 시작점/끝점
    bool bSelecting = false;
    FVector2D DragStart;
    FVector2D DragEnd;

    UFUNCTION()
    void OnPreviewFormation(const TArray<FFormationData>& Forms, const TArray<USquadComponent*>& Squads, bool bFinal);

protected:
    virtual void DrawHUD() override;
    TOptional<FFormationPreview> Preview;
    TArray<TWeakObjectPtr<USquadComponent>> HighlightedSquads;
};
