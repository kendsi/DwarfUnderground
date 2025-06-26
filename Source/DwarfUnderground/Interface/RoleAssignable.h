// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RoleAssignable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URoleAssignable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DWARFUNDERGROUND_API IRoleAssignable
{
	GENERATED_BODY()
	
public:
    // 역할을 할당하는 함수
    virtual void AssignRole(FName NewRole) = 0;
    // 현재 할당된 역할을 반환하는 함수
    virtual FName GetRole() const = 0;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
