// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildInteractionComponent.generated.h"

class UInputAction;
class ADwarfUndergroundBaseTile;
class ADwarfUndergroundBuild;
class ADwarfUndergroundPlayerController;

UENUM() enum class EBuildMode :uint8 { Build, Destroy };

UCLASS( ClassGroup=(Input), meta=(BlueprintSpawnableComponent) )
class UBuildInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    void Bind(class UEnhancedInputComponent* EIC, const UInputAction* LClick, const UInputAction* RClick,
        const UInputAction* Rotation, const UInputAction* ModeChange, const UInputAction* DestroyModeChange, const UInputAction* ESC);
    void SetMode(EBuildMode M) { Mode = M; }


private:

    EBuildMode Mode;
    ADwarfUndergroundPlayerController* CachedPC = nullptr;

    void OnLeftClick();
    void RotateBuilding();
    void HandleModeChange();
    void HandleBuildDestroyModeChange();
    void HandleBuild(const FHitResult& Hit);
    void HandleDestroy(const FHitResult& Hit);
    void HandleESC();
};