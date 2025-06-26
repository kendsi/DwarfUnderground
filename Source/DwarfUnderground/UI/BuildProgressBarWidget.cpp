// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildProgressBarWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/Widget.h"

void UBuildProgressBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    //자식 Progress Bar 찾기
    UWidget* wp = GetWidgetFromName(TEXT("Progressbar"));
    if (wp)
    {
        ProgressBar = Cast<UProgressBar>(wp);
        ProgressBar->SetPercent(0.0f);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("빌딩 UI: ProgressBar is nullptr"));
    }

    //자식 Text 찾기
    UWidget* tt = GetWidgetFromName(TEXT("ToolTip"));
    if (tt)
    {
        TextBlock = Cast<UTextBlock>(tt);
        TextBlock->SetText(FText::FromString(TEXT("0%")));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("빌딩 UI: ProgressBar is nullptr"));
    }
}

void UBuildProgressBarWidget::BeginProgress()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UBuildProgressBarWidget::UpdateProgress(float Percent)
{
    if (ProgressBar)
    {
        ProgressBar->SetPercent(Percent);
        TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%.0f%%"),Percent*100)));
    }
}

void UBuildProgressBarWidget::EndProgress()
{
    SetVisibility(ESlateVisibility::Hidden);
}
