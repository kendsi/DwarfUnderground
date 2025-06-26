// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ResourceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UResourceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DWARFUNDERGROUND_API IResourceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FName GetResourceType() const = 0;
	virtual int32 GetResourceAmount() const = 0;
	virtual int32 OnCollected(int32 amount) = 0;
};
