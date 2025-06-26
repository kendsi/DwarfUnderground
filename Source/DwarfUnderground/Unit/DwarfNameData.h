#pragma once

#include "CoreMinimal.h"
#include "DwarfNameData.generated.h"

/**
 * 드워프 이름 데이터베이스 형식
 * 
 */
USTRUCT(BlueprintType)
struct DWARFUNDERGROUND_API FDwarfNameData : public FTableRowBase
{	
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Meaning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Korean;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Sex;
};
