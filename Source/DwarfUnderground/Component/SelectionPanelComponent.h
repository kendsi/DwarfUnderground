// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DwarfUnderground/UI/InfoPanel.h"
#include "SelectionPanelComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DWARFUNDERGROUND_API USelectionPanelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USelectionPanelComponent();
	
	// 클릭 시 등장할 UI 패널
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInfoPanel> InfoPanelClass;
	
	//UI상 표기될 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	FString DisplayName;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	UTexture2D* BuildingImage;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
