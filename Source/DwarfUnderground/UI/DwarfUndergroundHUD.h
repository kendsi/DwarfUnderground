// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AllDwarfListPanel.h"
#include "DwarfDetailsPanel.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"
#include "DwarfUndergroundHUD.generated.h"


UCLASS()
class DWARFUNDERGROUND_API UDwarfUndergroundHUD : public UUserWidget
{
	GENERATED_BODY()



public:
	//블루프린트의 캔버스패널
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* CanvasPanel;
	
	//블루프린트의 팝업패널
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* PopupPanel;	
	
	//블루프린트 상 존재하는 우측 패널
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UAllDwarfListPanel* DwarfStatusPanel;

	//블루프린트 상 존재하는 DetailPanel
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDwarfDetailsPanel* DwarfDetailsPanel;

	//태그따라 생성할 UI 정보
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tags")
	//TMap<EBuildingType, TSubclassOf<UUserWidget>> BuildingWidgetMap;

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OpenBuildSelectionPanel();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CloseBuildSelectionPanel();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandleInventoryOpenButtonClicked();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandleDwarfManagementButtonClicked();

	UFUNCTION(BlueprintCallable)
	void OpenDwarfDetailsPanel(ADwarfUnit* TargetDwarf);

	UFUNCTION(BlueprintCallable)
	void HandleUnitClicked(AActor* SelectedActor);

	UFUNCTION(BlueprintCallable)
	void HandleWorldMouseDown();

	UFUNCTION(BlueprintCallable)
	void HandleUnitUnclicked();

	//B키나 버튼 눌렸을 때에 대한 대처
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandleBuildKeyClicked();
	
	//X키나 철거 버튼 눌렸을 때에 대한 대처
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandleBuildDestroyKeyClicked();

	UFUNCTION(BlueprintCallable)
	void CloseAllPopups(UWidget* ExceptFor = nullptr);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	USizeBox* BuildingDwarfCandidateSlot;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TogglePausePanel();

private:
	//TSubclassOf<UUserWidget> FindInWidgetMap(EBuildingType BuildingType);
	
	//InfoPanelSlot: 특정 조작으로 새로운 HUD 생성 시 나타날 위치
	UPROPERTY(meta = (BindWidget))
	USizeBox* BuildingInfoPanelSlot;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LogTextBlock;

	FString LastLog;
	float LastLogDurationInverse;
	float LogOpacity;

	//FloatingBuildingInfoHUD: 현재 떠 있는 빌딩 HUD
	UUserWidget* FloatingBuildingInfoHUD;
	

public:

	UFUNCTION(BlueprintCallable)
	static UDwarfUndergroundHUD* GetInstance();

	UFUNCTION(BlueprintCallable)
	void UILog(FString Text, float Duration, FLinearColor Color);

private:
	static TWeakObjectPtr<UDwarfUndergroundHUD> Singleton;
};
