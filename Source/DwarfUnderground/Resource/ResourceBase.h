// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResourceInterface.h"
#include "Containers/Set.h"
#include "ResourceBase.generated.h"

class ABuildingBase;
class AResourceWarehouse;
class ADwarfUnit;
class ADwarfAIController;

UCLASS()
class DWARFUNDERGROUND_API AResourceBase : public AActor, public IResourceInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResourceBase();

	UPROPERTY(BlueprintReadOnly)
	AResourceWarehouse* NearestWarehouse;

protected:

	UPROPERTY(EditAnywhere, Category = "Resource")
    FName ResourceType;

	UPROPERTY(EditAnywhere, Category = "Resource")
    int32 ResourceAmount;

	UPROPERTY()
	TSet<ADwarfAIController*> AssignedAIUnits;

public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual FName GetResourceType() const override { return ResourceType; }
    virtual int32 OnCollected(int32 amount) override;
    virtual int32 GetResourceAmount() const override { return ResourceAmount; }

	void SetResourceType(FName NewType) { ResourceType = NewType; }
	void SetResourceAmount(int32 Amount) { ResourceAmount = Amount; }

	void AssignAI(ADwarfAIController* AIController);
	void UnassignAI(ADwarfAIController* AIController);
	void NotifyResourceDepleted();
	
	UFUNCTION(BlueprintCallable)
	void UpdateNearestResourceWarehouse(const TArray<ABuildingBase*>& BulidingList);

	//건물의 입구 위치를 받아옴. 입구가 없다면 ActorLocation을 반환
	UFUNCTION(BlueprintCallable, Category = "DwarfManagement")
	FVector GetEntrancePosition();

protected:
	//대상 드워프의 자원을 나로 할당
	UFUNCTION()
	void AssignDwarfResource(ADwarfUnit* Dwarf);
	//대상 드워프의 자원을 없앰
	UFUNCTION()
	void UnassignDwarfResource(ADwarfUnit* Dwarf);

	//건물의 입구 위치(있을수도 없을수도 있음)
	TWeakObjectPtr<USceneComponent> Entrance;
	
};
