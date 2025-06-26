// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Buildable.generated.h"

class ADwarfUndergroundBaseTile;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UBuildable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Buildable: Actor의 인터페이스로 상속받아 사용합니다.
 *  - '건설' 관련 로직에서 해당 인터페이스를 검사하여 사용합니다.
 *  - ex: 흙, 건물, 장애물 등 '건설' 가능해야 하는 오브젝트
 *  블루프린트에서 IBuildable를 구현하는 액터만 찾고 싶다면 meta = (Implements = "Buildable") 식으로 사용
 *  사실 구조상으로는 굳이 Buildable이 없어도 되긴함: 추후 Tag 형식으로 바꿀지 논의 필요
 */
class DWARFUNDERGROUND_API IBuildable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	
public:
	//건물 생성 시 해당 건물의 크기를 확인하기 위함임.
	// - 위 목적 때문에 CDO의 GetWidth,Height를 사용하게 됨.
	// - 추후 필요시 변화 필요
	UFUNCTION(BlueprintCallable)
	virtual int32 GetWidth() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual int32 GetHeight() const = 0;

	//TODO: 순환참조 문제 해결 
	UFUNCTION(BlueprintCallable)
	virtual void AddOccupiedTile(ADwarfUndergroundBaseTile* Tile) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void RemoveOccupiedTile(ADwarfUndergroundBaseTile* Tile) = 0;

	//복사본이 전달됨
	UFUNCTION(BlueprintCallable)
	virtual TArray<ADwarfUndergroundBaseTile*> GetOccupiedTiles() = 0;

	UFUNCTION(BlueprintCallable)
	virtual AActor* GetActor() = 0;
};
