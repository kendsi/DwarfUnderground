// Fill out your copyright notice in the Description page of Project Settings.


#include "InfoWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"

void UInfoWidget::SetTargetActor(AActor* Target)
{
	TargetActor = Target;
	UE_LOG(LogTemp, Display, TEXT("BuildingInfoWidget: 빌딩 세팅됨: %s"),*GetName());
	OnBuildingSet(Target);

	//자식 컴포넌트 중 BuildingInfoWidget이 있다면 순회하여 알림
	if (WidgetTree)
	{
		TArray<UWidget*> Widgets;
		WidgetTree->GetAllWidgets(Widgets);
		for (auto Widget : Widgets)
		{
			if (UInfoWidget* BuildingInfoWidget = Cast<UInfoWidget>(Widget))
			{
				BuildingInfoWidget->SetTargetActor(Target);
			}
		}
	}	
}

AActor* UInfoWidget::GetTargetActor()
{
	return TargetActor;
}
