// Fill out your copyright notice in the Description page of Project Settings.

#include "DwarfAIController.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "DwarfUnderground/Equipment/EquipmentBase.h"
#include "DwarfUnderground/Interface/MinableInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "DwarfUnderground/Equipment/Weapon.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "DwarfUnderground/DwarfUndergroundPlayerController.h"

ADwarfAIController::ADwarfAIController()
{
    LastAttackTime = 0.0f;
	StorageLocation = FVector(2160, 740, 280);
    SetGenericTeamId(FGenericTeamId(0));      // 0 = Player/Dwarf
}

void ADwarfAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    UBlackboardComponent* BB = GetBlackboardComponent();
    BB->SetValueAsVector(StorageLocationKey, StorageLocation);
    ADwarfUnit* DU = Cast<ADwarfUnit>(InPawn);
    if (DU && DU->BehaviorTreeAsset)
    {
        RunBehaviorTree(DU->BehaviorTreeAsset);
        CachedCollision = DU->GetCapsuleComponent()->GetCollisionEnabled();
    }
}

void ADwarfAIController::AllocateWorkBuilding(ABuildingBase* Building)
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    BB->SetValueAsObject(WorkBuildingKey, Building);
    BB->SetValueAsBool(HasWorkBuildingKey, true);

    FVector BuildingEntrance = Building->GetEntrancePosition();
    BB->SetValueAsVector(WorkBuildingLocationKey, BuildingEntrance);

    ADwarfUnit* Self = Cast<ADwarfUnit>(GetPawn());
    CachedCollision = Self->GetCapsuleComponent()->GetCollisionEnabled();

    SetUnitState(EUnitState::Working);
}

void ADwarfAIController::DeallocateWorkBuilding()
{
    UBlackboardComponent* BB = GetBlackboardComponent();
    BB->SetValueAsObject(WorkBuildingKey, nullptr);
    BB->SetValueAsBool(HasWorkBuildingKey, false);

    ADwarfUnit* Self = Cast<ADwarfUnit>(GetPawn());
    Self->SetActorHiddenInGame(false);
    Self->GetCapsuleComponent()->SetCollisionEnabled(CachedCollision);

    SetUnitState(EUnitState::Idle);
}

void ADwarfAIController::AllocateResource(AResourceBase* Resource) {
    UBlackboardComponent* BB = GetBlackboardComponent();
    BB->SetValueAsObject(TargetResourceKey, Resource);

    //네비게이션상 입구를 등록
    FVector ResourceEntrance = Resource->GetEntrancePosition(); //빌딩 입구 위치 확인
    BB->SetValueAsVector(ResourceLocationKey, ResourceEntrance);
    
    SetUnitState(EUnitState::Gathering);
}

void ADwarfAIController::DeallocateResource() {
    UBlackboardComponent* BB = GetBlackboardComponent();
    BB->SetValueAsObject(TargetResourceKey, nullptr);
    SetUnitState(EUnitState::Idle);
}

void ADwarfAIController::SetStorageLocation(FVector Location) {
    StorageLocation = Location;
    UBlackboardComponent* BB = GetBlackboardComponent();
    BB->SetValueAsVector(StorageLocationKey, StorageLocation);
}

FVector ADwarfAIController::GetStorageLocation() {
    return StorageLocation;
}