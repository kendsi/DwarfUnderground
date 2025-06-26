#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "CameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCapsuleComponent;

UCLASS()
class DWARFUNDERGROUND_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ACameraPawn();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCapsuleComponent* CapsuleComponent;

	// 카메라 이동을 위한 스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComponent;

	// 실제 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	void MoveTriggered(const FInputActionValue& Value);
	void MoveCompleted(const FInputActionValue& Value);
	void CameraHeightTriggered(const FInputActionValue& Value);
	void RotationStarted(const FInputActionValue& Value);
	void RotationCompleted(const FInputActionValue& Value);
	void LookTriggered(const FInputActionValue& Value);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> CameraHeightAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> RotationAction;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly)
	float CameraHeightSpeed;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(EditDefaultsOnly)
	FVector2D ScreenEdgePadding;

	UPROPERTY(EditDefaultsOnly)
	FVector2D MovementSpeed;

	UPROPERTY(EditDefaultsOnly)
	float RotationSpeed;

	FVector2D CurrentInputMovementSpeed;
	bool ShouldRotate;
};
