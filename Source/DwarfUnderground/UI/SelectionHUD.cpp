// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectionHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "DwarfUnderground/Unit/Squad/SquadComponent.h"
#include "DwarfUnderground/Unit/Squad/SquadProxy.h"
#include "GameFramework/PlayerController.h"

void ASelectionHUD::DrawHUD()
{
    Super::DrawHUD();

    if (bSelecting)
    {
        // 드래그 박스 색상(녹색 반투명 예시)
        FLinearColor BoxColor(0.f, 1.f, 0.f, 0.25f);

        // 현재 마우스 위치가 DragEnd에 들어있다고 가정
        float Width = DragEnd.X - DragStart.X;
        float Height = DragEnd.Y - DragStart.Y;

        // 좌표가 음수가 될 수도 있으니, 사각형 그리기 전에 Min/Max 처리
        float X = (Width > 0) ? DragStart.X : DragEnd.X;
        float Y = (Height > 0) ? DragStart.Y : DragEnd.Y;
        float W = FMath::Abs(Width);
        float H = FMath::Abs(Height);

        // HUD가 제공하는 DrawRect 함수로 2D 사각형 표시
        DrawRect(BoxColor, X, Y, W, H);
    }

    if (!Preview) return;

    const float Dt = GetWorld()->GetDeltaSeconds();
    APlayerController* PC = GetWorld()->GetFirstPlayerController();

    for (int32 i = 0; i < Preview->Squads.Num(); i++) {
        USquadComponent* Squad = Preview->Squads[i];
        if (!Squad) continue;

        TArray<FVector> Slots;
        Squad->CalcSlots(Slots, Preview->Forms[i]);

        for (const FVector& WPos : Slots)
        {
            FVector2D Screen;
            PC->ProjectWorldLocationToScreen(WPos, Screen);

            FCanvasBoxItem Box(Screen - FVector2D(3, 3), FVector2D(6, 6));
            Box.SetColor(FLinearColor(0.f, 1.f, 1.f, Preview->Alpha));
            Canvas->DrawItem(Box);
        }
    }

    const FVector& A = Preview->Forms[0].Anchor;
    const FVector  FVec = Preview->Forms[0].ForwardDir * 150.f;

    auto DrawDir = [&](const FVector& From, const FVector& To)
    {
        FVector2D S, E;
        PC->ProjectWorldLocationToScreen(From, S);
        PC->ProjectWorldLocationToScreen(To, E);
        DrawLine(S.X, S.Y, E.X, E.Y, FLinearColor(0.f, 1.f, 1.f, Preview->Alpha));
    };
    DrawDir(A, A + FVec);

    // 사라짐 애니메이션
    if (Preview->bFinal)
    {
        Preview->Alpha -= Dt * 0.8f;      // 0.8 → 1.25 초에 사라짐
        if (Preview->Alpha <= 0.f) Preview.Reset();
    }
}

void ASelectionHUD::OnPreviewFormation(const TArray<FFormationData>& Forms,
    const TArray<USquadComponent*>& InSquads,
    bool bFinal)
{
    if (InSquads.Num() == 0) { Preview.Reset(); return; }

    Preview.Emplace();
    Preview->Forms = Forms;
    Preview->Squads = InSquads;
    Preview->Alpha = bFinal ? 0.7f : 0.7f;  // 시작 투명도
    Preview->bFinal = bFinal;
}