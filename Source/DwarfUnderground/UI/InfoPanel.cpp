// Fill out your copyright notice in the Description page of Project Settings.


#include "InfoPanel.h"

#include "AssignableDwarfList.h"
#include "DwarfUndergroundHUD.h"
#include "InfoWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBoxSlot.h"
#include "DwarfUnderground/Component/DwarfManagementComponent.h"

void UInfoPanel::NativeConstruct()
{
    Super::NativeConstruct();
    //SetVisibility(ESlateVisibility::Hidden);
}
void UInfoPanel::ClosePanel()
{
    //SetVisibility(ESlateVisibility::Hidden);
    //BuildingInfoSlot->ClearChildren();
}

void UInfoPanel::SetTargetActor(AActor* TargetBuilding)
{
    TargetActor = TargetBuilding;

    //자식 컴포넌트 중 BuildingInfoWidget이 있다면 순회하여 알림
    if (WidgetTree)
    {
        TArray<UWidget*> Widgets;
        WidgetTree->GetAllWidgets(Widgets);
        for (auto Widget : Widgets)
        {
            if (UInfoWidget* BuildingInfoWidget = Cast<UInfoWidget>(Widget))
            {
                BuildingInfoWidget->SetTargetActor(TargetActor);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildingInfoPanel: WidgetTree가 없습니다."));
    }

    OnTargetActorSet(TargetBuilding);
}

void UInfoPanel::OpenAssignableDwarfList()
{
    //위젯 생성
    auto* ListWidget = CreateWidget<UAssignableDwarfList>(GetOwningPlayer(), AssignableDwarfListClass );

    if (!ListWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("HUD: 할당가능 드워프 리스트 생성 실패"));
        return;
    }
    
    //위젯에서 드워프 선택될 경우 컴포넌트에 드워프 추가 요청 보냄
    ListWidget->OnDwarfSelected.AddDynamic(this, &UInfoPanel::RequestAddDwarf);

    //생성된 위젯을 HUD에 부착
    UDwarfUndergroundHUD* HUD = UDwarfUndergroundHUD::GetInstance();
    HUD->BuildingDwarfCandidateSlot->ClearChildren();
    HUD->BuildingDwarfCandidateSlot->AddChild(ListWidget);
    HUD->BuildingDwarfCandidateSlot->SetVisibility(ESlateVisibility::Visible);
}

void UInfoPanel::RequestAddDwarf(ADwarfUnit* DwarfUnit)
{
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("InfoPanel: 빌딩X"));
    }
    if (UDwarfManagementComponent* TargetComponent = TargetActor->GetComponentByClass<UDwarfManagementComponent>())
    {
        //드워프 할당 요청
        if (!TargetComponent->AddDwarf(DwarfUnit))
        {
            //TODO: 드워프 할당 실패 시
            UE_LOG(LogTemp, Warning, TEXT("InfoPanel: 드워프 할당 요청 실패, 드워프가 이미 가득 참"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("InfoPanel: 드워프 추가 요청을 보냈으나, 대상에 드워프 관리 컴포넌트가 없음"));
    }
}
