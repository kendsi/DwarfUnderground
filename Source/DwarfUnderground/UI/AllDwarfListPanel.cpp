// Fill out your copyright notice in the Description page of Project Settings.


#include "AllDwarfListPanel.h"

#include "DwarfUnderground/Unit/DwarfManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UAllDwarfListPanel::NativeConstruct()
{
    Super::NativeConstruct();
    IsRendered = false;
}

void UAllDwarfListPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (!IsRendered)
    {
        ReRenderPanel();
        IsRendered = true;
    }
}

void UAllDwarfListPanel::ReRenderPanel()
{
    //TODO: 모든 오브젝트 삭제 후 재생성 대신, 그냥 값만 업데이트, 드워프 수가 줄었을 경우 Hidden으로 변경하는 등

    //드워프 매니저에서 전체 드워프 확인
    auto* DwarfManager = GetGameInstance()->GetSubsystem<UDwarfManagerSubsystem>();
    UpdateDwarfList(DwarfManager->GetDwarves(EUnitState::Combat, EDwarfSortOrderBy::None));
    
    DwarfNum = Dwarves.Num();
}