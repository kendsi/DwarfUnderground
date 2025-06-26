// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfManagementComponent.h"

#include "DwarfUnderground/AI/Controller/DwarfAIController.h"
#include "DwarfUnderground/BuildManagement/Buildings/BuildingBase.h"
#include "DwarfUnderground/UI/LogUI.h"

// Sets default values for this component's properties
UDwarfManagementComponent::UDwarfManagementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//빌딩 블루프린트 생성 시 State Working으로 설정
	UnitState = EUnitState::Working;
	// ...
}


// Called when the game starts
void UDwarfManagementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UDwarfManagementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UDwarfManagementComponent::AddDwarf(ADwarfUnit* Dwarf)
{
	if (Dwarves.Num() >= MaxDwarfNum)
	{
		UE_LOG(LogTemp, Display, TEXT("DwarfManagementComponent: 드워프 할당 실패, 인원 초과"));
		return false;
	}

	//06.24 임시, 자원인데 저장소 없으면 아웃
	if (auto* ResourceBase = Cast<AResourceBase>(GetOwner()))
	{
		if (!ResourceBase->NearestWarehouse)
		{
			ULogUI::Log(TEXT("광물을 캐기 위해선 주변에 광산 창고가 필요합니다."));
			return false;	
		}
		//06.25 임시, 자원인데 곡괭이 없으면 아웃
		if (Dwarf->EquippedTool==nullptr)
		{
			ULogUI::Log(TEXT("광물을 캐기 위해선 유닛이 곡괭이를 장착중이어야 합니다."));
			return false;				
		}
	}

	//드워프 Manager에 추가 처리
	Dwarves.Add(Dwarf);
	Dwarf->SetUnitState(UnitState);
	Dwarf->SetWorkSite(GetOwner());

	//AIController에도 등록
	if (auto* CTR = Dwarf->GetController())
	{
		if (auto* AIController = Cast<ADwarfAIController>(CTR))
		{
			//AI컨트롤러에 내 액터가 빌딩이라면 빌딩에 등록, 자원이라면 자원으로 등록
			if (auto* Building = Cast<ABuildingBase>(GetOwner()))
			{
				AIController->AllocateWorkBuilding(Building);
				UE_LOG(LogTemp, Display, TEXT("DwarfManagement: 드워프 잘 할당됨"));
			}
			else if (auto* Resource = Cast<AResourceBase>(GetOwner()))
			{
				AIController->AllocateResource(Resource);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("DwarfManagement: 드워프 할당 실패, 할당 대상이 건물 자원 모두 아님"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DwarfManagement: 드워프 할당 실패, Controller가 AIController가 아님"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DwarfManagement: 드워프 할당 실패, GetController 실패"));
	}
	
	Dwarf->OnStateChanged.AddDynamic(this, &UDwarfManagementComponent::OnUnitStateChanged);

	OnDwarfListChanged.Broadcast(Dwarves);
	OnDwarfAdded.Broadcast(Dwarf);
	return true;
}

bool UDwarfManagementComponent::RemoveDwarf(ADwarfUnit* Dwarf)
{
	//드워프 Manager에 삭제 처리
	if (Dwarves.Contains(Dwarf))
	{
		Dwarves.Remove(Dwarf);
		//AIController의 빌딩을 삭제
		if (auto* CTR = Dwarf->GetController())
		{
			if (auto* AIController = Cast<ADwarfAIController>(CTR))
			{
				//AI컨트롤러에 내 액터가 빌딩이라면 빌딩에 등록, 자원이라면 자원으로 등록
				if (auto* Building = Cast<ABuildingBase>(GetOwner()))
				{
					AIController->DeallocateWorkBuilding();
				}
				else if (auto* Resource = Cast<AResourceBase>(GetOwner()))
				{
					AIController->DeallocateResource();
				}
			}
		}
		
		OnDwarfListChanged.Broadcast(Dwarves);
		OnDwarfRemoved.Broadcast(Dwarf);
		Dwarf->OnStateChanged.RemoveDynamic(this, &UDwarfManagementComponent::OnUnitStateChanged);

		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DwarfManagement: 드워프 삭제 요청 받았으나 이 컴포넌트의 드워프가 아님"));
		return false;
	}
}

void UDwarfManagementComponent::OnUnitStateChanged(AUnit* Unit, EUnitState NewState)
{
	if (NewState == EUnitState::Farming || NewState == EUnitState::Gathering || NewState == EUnitState::Recruiting ||
		NewState == EUnitState::Refining || NewState == EUnitState::Researching || NewState == EUnitState::Working ||
		NewState == EUnitState::EquipCrafting || NewState == EUnitState::SoilCovering || NewState ==
		EUnitState::SoilDigging) return;
	
	if (ADwarfUnit* DwarfUnit = Cast<ADwarfUnit>(Unit))
	{
		if (!RemoveDwarf(DwarfUnit))
		{
			//내 드워프가 아닌데 이벤트가 등록되었음. 강제로 해당 드워프에서 이벤트 제거
			DwarfUnit->OnStateChanged.RemoveDynamic(this, &UDwarfManagementComponent::OnUnitStateChanged);
			UE_LOG(LogTemp, Warning, TEXT("DwarfManagement: 내 드워프가 아닌 드워프에게서 StateChanged 이벤트 호출됨"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("삭제요청받은 유닛이 드워프가 아님"));
	}
}

void UDwarfManagementComponent::SignalDwarfUpdated()
{
	OnDwarfListChanged.Broadcast(Dwarves);
}


TArray<ADwarfUnit*> UDwarfManagementComponent::GetDwarves()
{
	return Dwarves;
}
