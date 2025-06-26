// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "DwarfUnderground/Component/DwarfManagementComponent.h"
#include "DwarfUnderground/Unit/DwarfUnit.h"
#include "RecruitingBuilding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecruitCompleted, ADwarfUnit*, TargetDwarf);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecruitListUpdated, const TArray<ADwarfUnit*>, RecruitedList);

/**
 * 
 */
UCLASS()
class DWARFUNDERGROUND_API ARecruitingBuilding : public ABuildingBase
{
	GENERATED_BODY()

public:
	//생성할 드워프
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dwarf)
	TSubclassOf<ADwarfUnit> DwarfBlueprint;
	
	//담당 모병관
	UDwarfManagementComponent* DwarfManagementComponent;

	//모병관별 모병 진척도
	TMap<ADwarfUnit*, double> RecruitingProgressMap;

	//고용 완료 이벤트
	FOnRecruitCompleted OnRecruitCompleted;

	//모병 목록 업데이트시 호출
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnRecruitListUpdated OnRecruitListUpdated;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	//모병 요청 함수
	UFUNCTION(BlueprintCallable)
	void Recruit(ADwarfUnit* Recruiter);

	//인원 고용
	UFUNCTION(BlueprintCallable)
	void Hire(ADwarfUnit* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Recruit")
	float GetRecruitProgress(const ADwarfUnit* Recruiter) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Recruit")
	const TArray<ADwarfUnit*>& GetRecruitedDwarves() const;

	
private:
	//작업중인 드워프, 진행율
	UPROPERTY()
	TMap<ADwarfUnit*, float> RecruitingProgress;

	//모병 완료된, 고용 대기중인 드워프
	UPROPERTY()
	TArray<ADwarfUnit*> RecruitedDwarves;

private:
	void CompleteRecruiting(const ADwarfUnit* Recruiter);

	UFUNCTION()
	void AddRecruiter(ADwarfUnit* Recruiter);

	UFUNCTION()
	void RemoveRecruiter(ADwarfUnit* Recruiter);

	
};