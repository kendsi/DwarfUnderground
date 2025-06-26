#pragma once

#include "CoreMinimal.h"
#include "NavModifierComponent.h"
#include "DwarfUndergroundNavBuilding.generated.h"

UCLASS()
class DWARFUNDERGROUND_API UDwarfUndergroundNavBuilding : public UNavModifierComponent
{
	GENERATED_BODY()

public:
	UDwarfUndergroundNavBuilding();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nav")
	float ExtentScale;

protected:
	virtual FBox GetNavigationBounds() const override;
};
