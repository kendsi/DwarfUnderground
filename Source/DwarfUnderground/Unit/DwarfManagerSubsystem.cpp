#include "DwarfManagerSubsystem.h"

#include "DwarfUnderground/DwarfUndergroundPlayerController.h"
#include "DwarfUnderground/Subsystem/DwarfDataSubsystem.h"
#include "Squad/SquadComponent.h"
#include "Kismet/GameplayStatics.h"

UDwarfManagerSubsystem::UDwarfManagerSubsystem()
{
	MinPersonnel = 3;
	MaxPersonnel = 12;
}

void UDwarfManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DwarfClass = StaticLoadClass(ADwarfUnit::StaticClass(), nullptr, TEXT("/Game/BluePrint/Unit/BP_DwarfUnit.BP_DwarfUnit_C"));
	
	//월드 로드 완료 시, 없는 드워프가 있다면 추가.
	//FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UDwarfManagerSubsystem::FindAllDwarvesInLevel);
}

void UDwarfManagerSubsystem::Deinitialize()
{
	Dwarves.Empty();
	Super::Deinitialize();
}

TArray<ADwarfUnit*> UDwarfManagerSubsystem::GetDwarves(EUnitState State, EDwarfSortOrderBy OrderBy) const
{
	//일단 다 보냄
	//TODO: 조건에 맞는 드워프만 찾아서 전송
	//TODO: 정렬 구현
	TArray<ADwarfUnit*> Temp;
	for (auto Dwarf : Dwarves)
	{
		Temp.Add(Dwarf);
	}
	return Temp;
}

void UDwarfManagerSubsystem::RegisterDwarf(ADwarfUnit* TargetDwarf)
{
	//드워프 랜덤 이름 생성
	//모병소 시스템 때문에 이름이 이미 있을 수도 있다. 이름이 없다면 할당한다.
	if (TargetDwarf->DwarfName==TEXT(""))
	{
		auto* NameGenerator = GetGameInstance()->GetSubsystem<UDwarfDataSubsystem>();
		FString DwarfName = NameGenerator->GetNextDwarfName().Korean;		
		TargetDwarf->DwarfName = DwarfName;
	}
	
	
	UE_LOG(LogTemp, Display, TEXT("Dwarf 등록됨: %s"),*TargetDwarf->GetName());
	Dwarves.Add(TargetDwarf);
	OnDwarfRegistered.Broadcast(TargetDwarf);

}

void UDwarfManagerSubsystem::UnregisterDwarf(ADwarfUnit* TargetDwarf)
{
	UE_LOG(LogTemp, Display, TEXT("Dwarf 삭제됨: %s"),*TargetDwarf->GetName());
	Dwarves.Remove(TargetDwarf);
	OnDwarfUnregistered.Broadcast(TargetDwarf);
}

bool UDwarfManagerSubsystem::CheckIsRegistered(ADwarfUnit* TargetDwarf)
{
	return Dwarves.Contains(TargetDwarf);
}

void UDwarfManagerSubsystem::FindAllDwarvesInLevel()
{
	//기존 목록 비움
	Dwarves.Empty();

	//모든 드워프 찾기
	TArray<AActor*> FoundDwarves;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADwarfUnit::StaticClass(), FoundDwarves);

	//추가
	for (auto FoundDwarf: FoundDwarves)
	{
		if (ADwarfUnit* Dwarf = Cast<ADwarfUnit>(FoundDwarf))
		{
			Dwarves.Add(Dwarf);
		}
	}
}

TArray<USquadComponent*> UDwarfManagerSubsystem::GetAllSquads()
{
	TArray<USquadComponent*> ToErase;
	for (auto* Squad : Squads)
	{
		if (Squad->Units.Num()<=0)
		{
			ToErase.Add(Squad);
		}
	}
	for (auto Squad : ToErase)
	{
		Squads.Remove(Squad);
	}

	UE_LOG(LogTemp, Display, TEXT("현재 Squad 수: %d"),Squads.Num());
	return Squads;
}

void UDwarfManagerSubsystem::RegisterSquad(USquadComponent* Squad)
{
	Squads.Add(Squad);
}
