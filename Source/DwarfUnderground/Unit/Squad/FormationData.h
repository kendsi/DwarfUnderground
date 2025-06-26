#pragma once

#include "CoreMinimal.h"
#include "FormationData.generated.h"

USTRUCT(BlueprintType)
struct FFormationData
{
	GENERATED_BODY()

	// 전열 중심
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Anchor = FVector::ZeroVector;
	// 전열 오른쪽 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RightDir = FVector::RightVector;
	// 전열 앞 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ForwardDir = FVector::ForwardVector;
	// 전열 폭
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float   FrontLineLength = 600.f;
	// 유닛 간 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float   Spacing = 100.f;
};