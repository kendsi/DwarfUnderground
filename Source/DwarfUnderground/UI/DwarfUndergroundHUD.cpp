// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfUndergroundHUD.h"

#include "InfoPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "DwarfUnderground/Component/SelectionPanelComponent.h"
#include "Kismet/GameplayStatics.h"


void UDwarfUndergroundHUD::NativeConstruct()
{
	Super::NativeConstruct();

	//Player Controller 할당 및 빌딩 클릭 이벤트 등록
	if (APlayerController* pc = GetOwningPlayer())
	{
		;
		if (ADwarfUndergroundPlayerController* PlayerController = Cast<ADwarfUndergroundPlayerController>(pc))
		{
			//05.06: 빌딩 패널 Main HUD에서 처리하기 위해 비활성화
			PlayerController->OnActorClicked.AddDynamic(this, &UDwarfUndergroundHUD::HandleUnitClicked);
			PlayerController->OnActorUnclicked.AddDynamic(this, &UDwarfUndergroundHUD::HandleUnitUnclicked);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BuildingInfoPanel: PlayerController를 Cast할 수 없습니다."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildingInfoPanel: PlayerController를 찾을 수 없습니다."));
	}

	//Static 변수 초기화
	if (Singleton == nullptr)
	{
		Singleton = this;
	}
	else
	{
		Singleton = this;
		UE_LOG(LogTemp, Warning, TEXT("HUD: HUD Instance가 2개 존재합니다."));
	}

	if (DwarfStatusPanel)
	{
		DwarfStatusPanel->OnElementClicked.AddDynamic(this, &UDwarfUndergroundHUD::OpenDwarfDetailsPanel);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HUD: 드워프 스테이터스 패널 찾을 수 없음"));
	}
}

void UDwarfUndergroundHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//로그 텍스트 업데이트
	if (LogOpacity > 0)
	{
		float AdaptOpacity = LogOpacity * 2.0f < 1.0f? LogOpacity*2.0f:1.0f;
		auto Color = LogTextBlock->GetColorAndOpacity().GetSpecifiedColor();
		auto Font = LogTextBlock->GetFont();

		Font.OutlineSettings.OutlineColor.A = AdaptOpacity * 0.8f;
		Color.A = AdaptOpacity;
		
		LogTextBlock->SetFont(Font);
		LogTextBlock->SetColorAndOpacity(FSlateColor(Color));

		LogOpacity -= LastLogDurationInverse * InDeltaTime;
		if (LogOpacity <= 0)
		{
			LogOpacity = 0;
			Color.A = 0.0f;
			LogTextBlock->SetColorAndOpacity(FSlateColor(Color));			
		}
	}

}

void UDwarfUndergroundHUD::OpenDwarfDetailsPanel(ADwarfUnit* TargetDwarf)
{
	UE_LOG(LogTemp, Display, TEXT("HUD: 드워프 디테일 패널 오픈 요청"));
	if (DwarfDetailsPanel)
	{
		//TODO: DwarfStatus 주입
		if (!TargetDwarf)
		{
			UE_LOG(LogTemp, Error, TEXT("Taret Dwarf 없음"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Target Dwarf: %s"), *TargetDwarf->GetName());
		}
		DwarfDetailsPanel->OpenPanel(TargetDwarf);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HUD: DwarfDetailsPanel 찾을 수 없음"));
	}
}

void UDwarfUndergroundHUD::HandleUnitClicked(AActor* SelectedActor)
{
	if (!SelectedActor) return;
	//적합한 HUD가 등록되어있지않으면 return
	USelectionPanelComponent* SelectionPanelComponent = SelectedActor->GetComponentByClass<USelectionPanelComponent>();
	if (!SelectionPanelComponent) return;

	TSubclassOf<UUserWidget> InfoPanelClass = SelectionPanelComponent->InfoPanelClass;
	if (InfoPanelClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("클릭한 Actor에 SelectionPanelComponent는 존재하나 PanelClass가 지정되지 않음"));
		return;
	}

	//기존 대상 삭제
	if (FloatingBuildingInfoHUD != nullptr)
	{
		FloatingBuildingInfoHUD->RemoveFromParent();
		FloatingBuildingInfoHUD = nullptr;
	}
	CloseAllPopups();
	//새로운 패널 추가
	FloatingBuildingInfoHUD = CreateWidget<UUserWidget>(GetWorld(), InfoPanelClass);
	UPanelSlot* PanelSlot = BuildingInfoPanelSlot->AddChild(FloatingBuildingInfoHUD);

	FloatingBuildingInfoHUD->SetVisibility(ESlateVisibility::Visible);
	BuildingInfoPanelSlot->SetVisibility(ESlateVisibility::Visible);

	//빌딩 할당
	if (UInfoPanel* BuildingInfoPanel = Cast<UInfoPanel>(FloatingBuildingInfoHUD))
	{
		BuildingInfoPanel->SetTargetActor(SelectedActor);
	}
	
	// 플레이어 컨트롤러 가져오기
	if (ADwarfUndergroundPlayerController* PC = Cast<ADwarfUndergroundPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		// 건물 선택 함수 호출
		PC->SelectBuilding(SelectedActor);
	}
}

void UDwarfUndergroundHUD::HandleWorldMouseDown()
{
	CloseAllPopups();
}

void UDwarfUndergroundHUD::HandleUnitUnclicked()
{
	UE_LOG(LogTemp, Display, TEXT("HUD target Unclicked"));
}

void UDwarfUndergroundHUD::CloseAllPopups(UWidget* ExceptFor)
{
	auto Children = PopupPanel->GetAllChildren();
	for (auto Child : Children)
	{
		if (Child == ExceptFor) continue;
		Child->SetVisibility(ESlateVisibility::Hidden);
	}

	if (ADwarfUndergroundPlayerController* PC = Cast<ADwarfUndergroundPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->DeselectCurrentBuilding();
	}
}

// TSubclassOf<UUserWidget> UDwarfUndergroundHUD::FindInWidgetMap(EBuildingType BuildingType)
// {
// 	//05.19 Changelist#347: 현재 미사용 함수
// 	return nullptr;
// 	//해당하는 태그가 있는지 찾고 반환한다.
// 	// if (TSubclassOf<UUserWidget>* FoundClass = BuildingWidgetMap.Find(BuildingType))
// 	// {
// 	// 	return *FoundClass;
// 	// }
// 	// else
// 	// {
// 	// 	return nullptr;
// 	// }
// }

TWeakObjectPtr<UDwarfUndergroundHUD> UDwarfUndergroundHUD::Singleton;


UDwarfUndergroundHUD* UDwarfUndergroundHUD::GetInstance()
{
	//HUD가 존재한다면 반환
	if (Singleton.IsValid())
	{
		return Singleton.Get();
	}
	return nullptr;
}

void UDwarfUndergroundHUD::UILog(FString Text, float Duration, FLinearColor Color)
{
	if (Duration <= 0.0f)
	{
		Duration = 0.0f;
		
		return;
	}
	
	LogTextBlock->SetText(FText::FromString(Text));
	LogTextBlock->SetColorAndOpacity(FSlateColor(Color));
	LastLogDurationInverse = 1/Duration;
	LastLog = Text;
	LogOpacity = 1.0f;
	
}
