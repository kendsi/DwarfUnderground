// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildInteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "DwarfUnderground/BuildManagement/DwarfUndergroundBaseTile.h"
#include "DwarfUnderground/DwarfUndergroundPlayerController.h"
#include "DwarfUnderground/BuildManagement/DwarfUndergroundBuildManager.h"
#include "DwarfUnderground/UI/DwarfUndergroundHUD.h"

void UBuildInteractionComponent::Bind(UEnhancedInputComponent* EIC, const UInputAction* L, const UInputAction* R,
                                      const UInputAction* Rotation, const UInputAction* ModeChange,
                                      const UInputAction* DestroyModeChange, const UInputAction* ESC)
{
	CachedPC = Cast<ADwarfUndergroundPlayerController>(GetOwner());
	EIC->BindAction(L, ETriggerEvent::Completed, this, &UBuildInteractionComponent::OnLeftClick);
	EIC->BindAction(Rotation, ETriggerEvent::Triggered, this, &UBuildInteractionComponent::RotateBuilding);
	EIC->BindAction(ModeChange, ETriggerEvent::Triggered, this, &UBuildInteractionComponent::HandleModeChange);
	EIC->BindAction(DestroyModeChange, ETriggerEvent::Completed, this,
	                &UBuildInteractionComponent::HandleBuildDestroyModeChange);
}

void UBuildInteractionComponent::OnLeftClick()
{
	FHitResult Hit;

	//06.12: 임시적으로 외부 클릭 시 UI 사라지게 하기 위해 구현
	UDwarfUndergroundHUD::GetInstance()->HandleWorldMouseDown();

	CachedPC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if (!Hit.bBlockingHit) return;

	(Mode == EBuildMode::Build) ? HandleBuild(Hit) : HandleDestroy(Hit);
}

void UBuildInteractionComponent::RotateBuilding()
{
	UE_LOG(LogTemp, Display, TEXT("RotateBuilding"));
}

void UBuildInteractionComponent::HandleBuildDestroyModeChange()
{
	auto* HUD = UDwarfUndergroundHUD::GetInstance();
	HUD->HandleBuildDestroyKeyClicked();
}

void UBuildInteractionComponent::HandleModeChange()
{
	//일반모드 시작
	CachedPC->SetBuildSelectionMode(2);
}

void UBuildInteractionComponent::HandleBuild(const FHitResult& Hit)
{
	auto* Tile = Cast<ADwarfUndergroundBaseTile>(Hit.GetActor());
	if (!Tile) return;

	// 플레이어 컨트롤러에서 현재 선택된 빌딩 타입 가져오기
	TSubclassOf<AActor> SelectedType = CachedPC->GetBuildingType();

	if (!SelectedType || !SelectedType->ImplementsInterface(UBuildable::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid build type"));
		return;
	}

	if (Tile->BuildOnTile(SelectedType))
	{
		UE_LOG(LogTemp, Log, TEXT("Tile build success"));
	}
}

void UBuildInteractionComponent::HandleDestroy(const FHitResult& Hit)
{
	if (!Hit.bBlockingHit) return;

	if (ABuildingBase* ClickedBuilding = Cast<ABuildingBase>(Hit.GetActor()))
	{
		if (ADwarfUndergroundBaseTile* Tile = Cast<ADwarfUndergroundBaseTile>(ClickedBuilding->GetOwner()))
		{
			Tile->RemoveBuilding();
			UE_LOG(LogTemp, Display, TEXT("Building Removed from Tile"));
		}
		return;
	}

	if (ADwarfUndergroundBaseTile* ClickedTile = Cast<ADwarfUndergroundBaseTile>(Hit.GetActor()))
	{
		ADwarfUndergroundBuildManager* BuildManagerRef = CachedPC->GetBuildManagerRef();
		if (!CachedPC || !BuildManagerRef) return;

		UE_LOG(LogTemp, Display, TEXT("Clicked Tile(%d,%d) SoilStack:%d"), ClickedTile->GridX, ClickedTile->GridY,
		       ClickedTile->SoilStack.Num());

		if (ClickedTile->SoilStack.Num() > 0)
		{
			BuildManagerRef->RemoveTopSoilFromTile(ClickedTile);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					2.0f,
					FColor::Cyan,
					TEXT("Soil Removed")
				);
			}
			UE_LOG(LogTemp, Display, TEXT("Soil Removed, New SoilStack count: %d"), ClickedTile->SoilStack.Num());
		}
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Clicked actor is neither a Building nor a Tile"));
}