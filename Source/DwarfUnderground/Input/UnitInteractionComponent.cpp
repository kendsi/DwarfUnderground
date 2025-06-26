// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitInteractionComponent.h"
#include "DwarfUnderground/UI/DwarfUndergroundHUD.h"
#include "Kismet/GameplayStatics.h"
#include "DwarfUnderground/Unit/Squad/SquadProxy.h"
#include "DwarfUnderground/Unit/Squad/SquadComponent.h"
#include "DwarfUnderground/UI/SelectionHUD.h"
#include "DwarfUnderground/DwarfUndergroundPlayerController.h"

// Sets default values for this component's properties
UUnitInteractionComponent::UUnitInteractionComponent()
{
    ClickThreshold = 4.0f;
    PixelToWorldScale = 5.0f;
    HUDRef = nullptr;
    PrimaryComponentTick.bCanEverTick = true;
    CachedPC = Cast<ADwarfUndergroundPlayerController>(GetOwner());
}

void UUnitInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
    if (CachedPC) {
        HUDRef = Cast<ASelectionHUD>(CachedPC->GetHUD());
        OnPreviewFormation.AddDynamic(HUDRef, &ASelectionHUD::OnPreviewFormation);
    }
}

void UUnitInteractionComponent::TickComponent(float Dt, ELevelTick, FActorComponentTickFunction*)
{
    if (bLDrag && HUDRef)
    {
        FVector2D Cur = FVector2D(0, 0);
        if (CachedPC)
            CachedPC->GetMousePosition(Cur.X, Cur.Y);

        HUDRef->DragEnd = Cur;
    }

    if (bRDrag && CurrSel.Num())
    {
        FVector2D Now;
        Cast<APlayerController>(GetOwner())->GetMousePosition(Now.X, Now.Y);
        TArray<FFormationData> PreviewForms;
        FFormationData PreviewForm = CalcFormationFromScreenLine(RS, Now);
        PreviewForm.FrontLineLength = PreviewForm.FrontLineLength / CurrSel.Num();

        float Temp = 0.f;
        for (USquadComponent* Squad : CurrSel) {
            if (!Squad)
                continue;
            PreviewForm.Anchor = PreviewForm.Anchor + PreviewForm.RightDir * Temp;
            PreviewForms.Add(PreviewForm);
            Temp = Temp + PreviewForm.FrontLineLength;
        }
        OnPreviewFormation.Broadcast(PreviewForms, CurrSel, false); // 드래그 중
    }
}

void UUnitInteractionComponent::Bind(UEnhancedInputComponent* EIC, const UInputAction* L, const UInputAction* R,
    const UInputAction* Select, const UInputAction* Info, const UInputAction* Inventory, const UInputAction* ModeChange, const UInputAction* ESC)
{
    EIC->BindAction(L, ETriggerEvent::Started, this, &UUnitInteractionComponent::LDragStart);
    EIC->BindAction(L, ETriggerEvent::Completed, this, &UUnitInteractionComponent::LDragEnd);
    EIC->BindAction(R, ETriggerEvent::Started, this, &UUnitInteractionComponent::RDragStart);
    EIC->BindAction(R, ETriggerEvent::Completed, this, &UUnitInteractionComponent::RDragEnd);
    EIC->BindAction(Select, ETriggerEvent::Triggered, this, &UUnitInteractionComponent::SelectSquad);
    EIC->BindAction(Info, ETriggerEvent::Triggered, this, &UUnitInteractionComponent::ShowDwarfInfo);
    EIC->BindAction(Inventory, ETriggerEvent::Triggered, this, &UUnitInteractionComponent::ShowInventory);
    EIC->BindAction(ModeChange, ETriggerEvent::Completed, this, &UUnitInteractionComponent::HandleModeChange);
    EIC->BindAction(ESC, ETriggerEvent::Completed, this, &UUnitInteractionComponent::HandleESC);
}

void UUnitInteractionComponent::SelectSquad(int32 SquadNum)
{
    TArray<USquadComponent*> Squads = CachedPC->GetSquadList();
    if (Squads.IsValidIndex(SquadNum) && Squads[SquadNum])
    {
        for (USquadComponent* Squad : CurrSel)
            Squad->Proxy->OnDeselected();
        CurrSel.Empty();
        CurrSel.Add(Squads[SquadNum]);
        Squads[SquadNum]->Proxy->OnSelected();
    }
}

const TArray<USquadComponent*>& UUnitInteractionComponent::GetCurrentSelectedSquad()
{
    TArray<USquadComponent*> InvalidSquads;
    for (auto* Target: CurrSel)
    {
        if (!IsValid(Target))
        {
            InvalidSquads.Add(Target);
        }
    }
    for (auto* Squad : InvalidSquads)
    {
        CurrSel.Remove(Squad);
    }
    return CurrSel;
}

// 좌클릭 / 좌드래그
void UUnitInteractionComponent::LDragStart()
{
    if (CachedPC)
    {
        CachedPC->GetMousePosition(LS.X, LS.Y);
        bLDrag = true;

        if (HUDRef)
        {
            HUDRef->bSelecting = true;
            HUDRef->DragStart = LS;
            HUDRef->DragEnd = LS;
        }
    }
}

void UUnitInteractionComponent::LDragEnd()
{
    if (!bLDrag) return;
    bLDrag = false;

    if (HUDRef) HUDRef->bSelecting = false;

    CachedPC->GetMousePosition(LE.X, LE.Y);

    const float Dist = (LE - LS).Size();
    TArray<USquadComponent*> Result;

    for (USquadComponent* Squad : CurrSel) {
        if (IsValid(Squad))
            Squad->Proxy->OnDeselected();
    }

    //06.12: 임시적으로 외부 클릭 시 UI 사라지게 하기 위해 구현
    UDwarfUndergroundHUD::GetInstance()->HandleWorldMouseDown();    

    if (Dist < ClickThreshold) // 단일 클릭
    {
        FHitResult Hit;
        CachedPC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);

        //빌딩 UI 생성용
        UDwarfUndergroundHUD::GetInstance()->HandleUnitClicked(Hit.GetActor());

        if (AUnit* Unit = Cast<AUnit>(Hit.GetActor()))
            if (USquadComponent* Squad = Unit->SquadRef)
                Result.Add(Squad);
    }
    else                          // 박스 드래그
    {
        GatherSquads(LS, LE, Result);
    }

    CurrSel = Result;
    // HandleSelect(Result);
}

// 우클릭 / 우드래그
void UUnitInteractionComponent::RDragStart()
{
    if (CurrSel.Num() == 0) return;

    if (CachedPC)
    {
        CachedPC->GetMousePosition(RS.X, RS.Y);
        bRDrag = true;
    }
}

void UUnitInteractionComponent::RDragEnd()
{
    if (!bRDrag) return;
    bRDrag = false;

    if (!CachedPC) return;

    FVector2D EndScreen;
    CachedPC->GetMousePosition(EndScreen.X, EndScreen.Y);
    float Dist = (EndScreen - RS).Size();

    if (Dist < ClickThreshold)
    {
        FHitResult Hit;

        // 유닛 전용 채널로 먼저 검사
        CachedPC->GetHitResultUnderCursor(ECC_Pawn, false, Hit);
        if (Hit.bBlockingHit && Hit.GetActor())
        {
            if (AUnit* EnemyUnit = Cast<AUnit>(Hit.GetActor()))
            {
                if (EnemyUnit->SquadRef) {
                    for (USquadComponent* Squad : CurrSel)
                        Squad->SetEnemySquad(EnemyUnit->SquadRef);
                }
                else {
                    for (USquadComponent* Squad : CurrSel)
                        Squad->SetEnemyUnit(EnemyUnit);
                }
                return;
            }
        }


        CachedPC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
        if (!Hit.bBlockingHit) return;

        TArray<FFormationData> Forms;
        float Temp = 0.f;
        for (USquadComponent* Squad : CurrSel)
        {
            if (!Squad) continue;
            // 각 스쿼드가 기억한 마지막 포메이션
            FFormationData Form = Squad->GetLastFormation();
            // 앵커만 클릭 지점으로 교체
            Form.Anchor = Hit.Location + Form.RightDir * Temp;
            Forms.Add(Form);
            Squad->ForceMove(Form);
            Temp = Temp + Form.FrontLineLength;
        }
        OnPreviewFormation.Broadcast(Forms, CurrSel, true);    // 마우스 놓음
    }
    // 드래그(거리 큼) → 새로운 포메이션 계산 & 이동
    else
    {
        TArray<FFormationData> Forms;
        FFormationData Form = CalcFormationFromScreenLine(RS, EndScreen);
        Form.FrontLineLength = Form.FrontLineLength / CurrSel.Num();
        float Temp = 0.f;
        // 3) 각 스쿼드에 포메이션 이동 요청
        for (USquadComponent* Squad : CurrSel)
        {
            if (!Squad) continue;
            Form.Anchor = Form.Anchor + Form.RightDir * Temp;
            Forms.Add(Form);
            Squad->ForceMove(Form);
            Temp = Temp + Form.FrontLineLength;
        }
        OnPreviewFormation.Broadcast(Forms, CurrSel, true);    // 마우스 놓음
    }
}

void UUnitInteractionComponent::GatherSquads(const FVector2D& A, const FVector2D& B, TArray<USquadComponent*>& Out) const
{
    FVector2D Min(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y));
    FVector2D Max(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y));

    TArray<AActor*> Proxies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASquadProxy::StaticClass(), Proxies);

    if (!CachedPC) return;

    for (AActor* Act : Proxies)
    {
        ASquadProxy* Proxy = Cast<ASquadProxy>(Act);
        FVector2D Scr;
        CachedPC->ProjectWorldLocationToScreen(Act->GetActorLocation(), Scr, true);

        if (Scr.X >= Min.X && Scr.X <= Max.X &&
            Scr.Y >= Min.Y && Scr.Y <= Max.Y &&
            !Proxy->bIsStatic)
        {
            Proxy->OnSelected();
            USquadComponent* Squad = Proxy->OwnerSquad;
            Out.Add(Squad);
        }
    }
}

FFormationData UUnitInteractionComponent::CalcFormationFromScreenLine(
    const FVector2D& ScreenA,       // 우클릭 드래그 시작 지점
    const FVector2D& ScreenB) const // 현재(또는 끝) 지점
{
    FFormationData Form;            // 기본값으로 초기화됨

    //― 1. 두 스크린 좌표 → 지면(world) 교차점
    if (!CachedPC) return Form;

    FHitResult HitA, HitB;
    CachedPC->GetHitResultAtScreenPosition(ScreenA, ECC_Visibility, /*bTraceComplex*/false, HitA);
    CachedPC->GetHitResultAtScreenPosition(ScreenB, ECC_Visibility, false, HitB);
    if (!HitA.bBlockingHit || !HitB.bBlockingHit) return Form;   // 땅을 못 찍었으면 빈 값

    const FVector Left = HitA.Location;
    const FVector Right = HitB.Location;

    //― 2. 전열(Front-Line) 벡터와 길이
    const FVector Delta = Right - Left;
    const float   FrontLen = Delta.Size2D();           // 폭
    const FVector RightDir = Delta.GetSafeNormal2D();  // 오른쪽 방향
    const FVector ForwardDir = FVector::CrossProduct(RightDir, FVector::UpVector); // 전방 = Up × Right

    //― 3. 결과 채우기
    Form.Anchor = (Left + Right) * 0.5f;  // 전열 중앙
    Form.RightDir = RightDir;
    Form.ForwardDir = ForwardDir;
    Form.FrontLineLength = FrontLen;

    return Form;
}

void UUnitInteractionComponent::SelectSquad(const FInputActionValue& Value) {
    int32 Index = FMath::RoundToInt(Value.Get<float>());   // 0~9
    SelectSquad(Index);
}

void UUnitInteractionComponent::ShowDwarfInfo() {
    
    auto* HUD = UDwarfUndergroundHUD::GetInstance();
    HUD->HandleDwarfManagementButtonClicked();
    UE_LOG(LogTemp, Display, TEXT("DwarfInfo"));
}

void UUnitInteractionComponent::ShowInventory() {
    
    auto* HUD = UDwarfUndergroundHUD::GetInstance();
    HUD->HandleInventoryOpenButtonClicked();
    UE_LOG(LogTemp, Display, TEXT("Inventory"));
}

void UUnitInteractionComponent::HandleModeChange() {
    //CachedPC->SetBuildSelectionMode(0);
    auto* HUD = UDwarfUndergroundHUD::GetInstance();
    HUD->HandleBuildKeyClicked();
    UE_LOG(LogTemp, Display, TEXT("Build Button"));
    
}


void UUnitInteractionComponent::HandleSelect(const TArray<USquadComponent*>& SelectedSquads)
{
}

void UUnitInteractionComponent::HandleMove(const TArray<USquadComponent*>& SelectedSquads, const FVector& Destination)
{
    for (auto* Squad : SelectedSquads)
        Squad->ForceMove({ Destination, /*…포메이션 기본값…*/ });
}

void UUnitInteractionComponent::HandleFormMove(const TArray<USquadComponent*>& SelectedSquads, const FFormationData& FormationData)
{
    for (auto* Squad : SelectedSquads)
        Squad->ForceMove(FormationData);
}

void UUnitInteractionComponent::HandleESC() {
    UE_LOG(LogTemp, Warning, TEXT("ESC"));
    if (auto* HUD = UDwarfUndergroundHUD::GetInstance())
        HUD->TogglePausePanel();
}