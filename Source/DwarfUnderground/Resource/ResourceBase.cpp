// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceBase.h"
#include "DwarfUnderground/AI/Controller/DwarfAIController.h"
#include "DwarfUnderground/BuildManagement/Buildings/ResourceWarehouse.h"
#include "DwarfUnderground/Component/DwarfManagementComponent.h"
#include "DwarfUnderground/Subsystem/BuildingManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

// Sets default values
AResourceBase::AResourceBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ResourceAmount = 100;
}

void AResourceBase::BeginPlay()
{
	Super::BeginPlay();

	//유닛이 자원에 등록될 경우를 대비해, DwarfManagementComponent(유닛 등록 관리)컴포넌트에 이벤트 등록
	auto* DwarfManager = GetComponentByClass<UDwarfManagementComponent>();
	if (!DwarfManager)
	{
		UE_LOG(LogTemp, Error, TEXT("ResourceBase: DwarfManagementComponent를 찾을 수 없음"));
		return;
	}
	DwarfManager->OnDwarfAdded.AddDynamic(this, &AResourceBase::AssignDwarfResource);
	DwarfManager->OnDwarfRemoved.AddDynamic(this, &AResourceBase::UnassignDwarfResource);

	//빌딩들이 업데이트될 때마다 가장 가까운 창고를 찾는다.
	if (UGameInstance* GI = GetGameInstance())
	{
		UBuildingManagerSubsystem* BuildingManager = GI->GetSubsystem<UBuildingManagerSubsystem>();
		BuildingManager->OnBuildingListUpdated.AddDynamic(this, &AResourceBase::UpdateNearestResourceWarehouse);
	}
}

void AResourceBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (UGameInstance* GI = GetGameInstance())
	{
		UBuildingManagerSubsystem* BuildingManager = GI->GetSubsystem<UBuildingManagerSubsystem>();
		BuildingManager->OnBuildingListUpdated.RemoveDynamic(this, &AResourceBase::UpdateNearestResourceWarehouse);
	}
}

int32 AResourceBase::OnCollected(int32 amount)
{
	if (ResourceAmount <= 0)
		return 0;

	ResourceAmount -= amount;
	if (ResourceAmount <= 0)
	{
		NotifyResourceDepleted();
		Destroy();
		return ResourceAmount + amount;
	}

	return amount;
}

void AResourceBase::AssignAI(ADwarfAIController* AIController)
{
	if (AIController)
	{
		AssignedAIUnits.Add(AIController);
	}
}

void AResourceBase::UnassignAI(ADwarfAIController* AIController)
{
	if (AIController)
	{
		AssignedAIUnits.Remove(AIController);
	}
}

void AResourceBase::NotifyResourceDepleted()
{
	for (ADwarfAIController* AICon : AssignedAIUnits)
	{
		AICon->DeallocateResource();
	}
}

void AResourceBase::UpdateNearestResourceWarehouse(const TArray<ABuildingBase*>& BulidingList)
{
	//모든 빌딩 리스트 중 창고만 찾아낸다.
	TArray<AActor*> Warehouses;
	for (auto* Building : BulidingList)
	{
		if (Building->IsA(AResourceWarehouse::StaticClass()))
		{
			Warehouses.Add(Building);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Resource: 파악된 Warehouse 수: %d"),Warehouses.Num());

	//가장 가까운 창고를 찾는다.
	double Dist = -1.0f;
	AActor* Target = nullptr;
	for (AActor* Warehouse : Warehouses)
	{
		double Test = FVector::DistSquared(Warehouse->GetActorLocation(), GetActorLocation());
		if (Dist > Test || Dist == -1.0f)
		{
			Dist = Test;
			Target = Warehouse;
		}
	}
	if (Target == nullptr)
	{
		NearestWarehouse = nullptr;
	}
	else
	{
		AResourceWarehouse* TargetWarehouse = Cast<AResourceWarehouse>(Target);
		NearestWarehouse = TargetWarehouse;
	}
}

FVector AResourceBase::GetEntrancePosition()
{
	if (Entrance.IsValid())
	{
		return Entrance.Get()->GetComponentLocation();
	}
	else
	{
		TArray<UActorComponent*> Components = GetComponentsByTag(USceneComponent::StaticClass(), FName("Entrance"));
		if (Components.Num() > 0)
		{
			if (auto* Ent = Cast<USceneComponent>(Components[0]))
			{
				Entrance = Ent;
				return Entrance->GetComponentLocation();
			}
		}
		return GetActorLocation();
	}
}

void AResourceBase::AssignDwarfResource(ADwarfUnit* Dwarf)
{
	UE_LOG(LogTemp, Display, TEXT("ResourceBase: 드워프 할당됨!"));
	AController* Controller = Dwarf->GetController();
	if (!Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResourceBase: 드워프가 할당되었으나 (AI)Controller가 없습니다."));
		return;
	}

	if (ADwarfAIController* AIController = Cast<ADwarfAIController>(Controller))
	{
		if (!NearestWarehouse)
		{
			UE_LOG(LogTemp, Warning, TEXT("ResourceBase: 자원 주변에 Warehouse가 없습니다."));
			return;
		}

		FNavLocation Dest;
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
		FVector NearestWarehouseEntrance = NearestWarehouse->GetEntrancePosition();
		if (NavSys->ProjectPointToNavigation(NearestWarehouseEntrance, Dest, FVector(300.0f, 300.0f, 100.0f)))
			AIController->SetStorageLocation(Dest.Location);
		else
			UE_LOG(LogTemp, Warning, TEXT("StorageLocation 설정 실패"));
		AIController->AllocateResource(this);
		AssignAI(AIController);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ResourceBase: 드워프가 할당되었으나 (AI)Controller가 없습니다."));
		return;
	}
}

void AResourceBase::UnassignDwarfResource(ADwarfUnit* Dwarf)
{
	AController* Controller = Dwarf->GetController();
	ADwarfAIController* AIController = nullptr;
	if (Controller)
	{
		AIController = Cast<ADwarfAIController>(Controller);
	}

	if (!Controller || !AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResourceBase: 드워프가 삭제 요청: (AI)Controller가 없어 AI 처리 불가."));
		return;
	}

	AIController->DeallocateWorkBuilding();
	AIController->DeallocateResource();
	UnassignAI(AIController);
}
