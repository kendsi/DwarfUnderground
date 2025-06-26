// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Unit/DwarfUnit.h"
#include "DwarfManagementComponent.generated.h"

class UDwarfAssignableHolder;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDwarfManagingListChanged, const TArray<ADwarfUnit*>&, DwarfList);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDwarfAdded,ADwarfUnit*, AddedDwarf);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDwarfRemoved,ADwarfUnit*, RemovedDwarf);

//건물, 자원 등 드워프를 할당 가능한 곳은 이 컴포넌트를 가지고 있어야함
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DWARFUNDERGROUND_API UDwarfManagementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDwarfManagementComponent();

	UPROPERTY(BlueprintAssignable)
	FDwarfManagingListChanged OnDwarfListChanged;

	UPROPERTY(BlueprintAssignable)
	FOnDwarfAdded OnDwarfAdded;

	UPROPERTY(BlueprintAssignable)
	FOnDwarfRemoved OnDwarfRemoved;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DwarfManagement")
	int32 MaxDwarfNum;

	//할당된 드워프가 어떤 상태로 전이되는지 정의합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DwarfManagement", meta=(ToolTip = "할당된 드워프가 어떤 상태로 전이되는지 정의합니다."))
	EUnitState UnitState;
	
	//드워프 Management UI에 드워프가 할당되면 어떤 클래스를 Render할 지 결정합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DwarfManagement", meta=(ToolTip = "드워프 Management UI에 드워프가 할당되면 어떤 클래스를 Render할 지 결정합니다."))
	TSubclassOf<UDwarfAssignableHolder> DwarfAssignedSlotClass;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "DwarfManagement")
	virtual bool AddDwarf(ADwarfUnit* Dwarf);
	UFUNCTION(BlueprintCallable, Category = "DwarfManagement")
	virtual bool RemoveDwarf(ADwarfUnit* Dwarf);
	//외부에 의해 Unit State 변경 시에 대한 콜백 함수
	UFUNCTION(BlueprintCallable, Category = "DwarfManagement")
	virtual void OnUnitStateChanged(AUnit* Unit, EUnitState NewState);

	UFUNCTION(BlueprintCallable, Category = "DwarfManagement")
	void SignalDwarfUpdated();

	UFUNCTION(BlueprintCallable, Category = "DwarfManagement")
	virtual TArray<ADwarfUnit*> GetDwarves();
	

protected:
	TArray<ADwarfUnit*> Dwarves;
};
