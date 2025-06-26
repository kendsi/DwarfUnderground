#pragma once

#include "CoreMinimal.h"
#include "BuildingPriceData.generated.h"
class ABuildingBase;
/**
 * UDataTable에 FItemData를 여러개 넣어두고 ItemID로 조회
 * DataTable을 애셋 경로에서 로드하거나, GetDataTableRowFromName()함수 사용하여 접근
 */
USTRUCT(BlueprintType)
struct DWARFUNDERGROUND_API FBuildingPriceData : public FTableRowBase
{
	GENERATED_BODY()	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> BuildingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, int32> RequiredItem;
};