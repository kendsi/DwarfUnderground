// Fill out your copyright notice in the Description page of Project Settings.


#include "RecruitingBuilding.h"

#include "DwarfUnderground/Subsystem/DwarfDataSubsystem.h"
#include "DwarfUnderground/Unit/DwarfManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

void ARecruitingBuilding::BeginPlay()
{
	Super::BeginPlay();
	if (auto* Management = GetComponentByClass<UDwarfManagementComponent>())
	{
		Management->OnDwarfAdded.AddDynamic(this, &ARecruitingBuilding::AddRecruiter);
		Management->OnDwarfRemoved.AddDynamic(this, &ARecruitingBuilding::RemoveRecruiter);
	}
}

void ARecruitingBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//현재 할당된 드워프의 모병 할당 진행
	TArray<ADwarfUnit*> CompletedUnits;
	for (auto Iter = RecruitingProgress.CreateIterator(); Iter; ++Iter)
	{
		if (Iter->Value >= 1.0f)
		{
			CompletedUnits.Add(Iter->Key);
		}
		else
		{
			Iter->Value += DeltaTime * 0.3f;
		}
	}
	for (auto* Unit : CompletedUnits)
	{
		RecruitingProgress[Unit] -= 1.0f;
		Recruit(Unit);
	}
}

void ARecruitingBuilding::Recruit(ADwarfUnit* Recruiter)
{
	//1. 드워프 Deferred 생성
	//2. 해당 드워프를 List에 등록
	//3. ListUpdated 호출
	UE_LOG(LogTemp, Display, TEXT("모병 완료"));

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation()+FVector(0,0,-30000));

	AActor* DeferredActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(
		GetWorld(),
		DwarfBlueprint,
		SpawnTransform,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn,
		/*Owner=*/this,
		ESpawnActorScaleMethod::SelectDefaultAtRuntime
	);

	if (auto* DeferredDwarf = Cast<ADwarfUnit>(DeferredActor))
	{
		auto* NameGenerator = GetGameInstance()->GetSubsystem<UDwarfDataSubsystem>();
		FString DwarfName = NameGenerator->GetNextDwarfName().Korean;
		DeferredDwarf->DwarfName = DwarfName;

		RecruitedDwarves.Add(DeferredDwarf);
		OnRecruitCompleted.Broadcast(DeferredDwarf);
		OnRecruitListUpdated.Broadcast(RecruitedDwarves);
	}
}

void ARecruitingBuilding::Hire(ADwarfUnit* Target)
{
	UE_LOG(LogTemp, Display, TEXT("고용 요청됨"));

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetEntrancePosition());


	UGameplayStatics::FinishSpawningActor(Target, SpawnTransform);

	RecruitedDwarves.Remove(Target);
	OnRecruitListUpdated.Broadcast(RecruitedDwarves);
}


float ARecruitingBuilding::GetRecruitProgress(const ADwarfUnit* Recruiter) const
{
	if (RecruitingProgress.Contains(Recruiter))
	{
		return RecruitingProgress[Recruiter];
	}
	return 0.0f;
}


const TArray<ADwarfUnit*>& ARecruitingBuilding::GetRecruitedDwarves() const
{
	return RecruitedDwarves;
}


void ARecruitingBuilding::AddRecruiter(ADwarfUnit* Recruiter)
{
	RecruitingProgress.Add(Recruiter, 0.0f);
}

void ARecruitingBuilding::RemoveRecruiter(ADwarfUnit* Recruiter)
{
	RecruitingProgress.Remove(Recruiter);
}
