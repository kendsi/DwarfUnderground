#include "CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"

ACameraPawn::ACameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(/*Radius=*/32.f, /*HalfHeight=*/48.f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	RootComponent = CapsuleComponent;

	// 스프링 암 생성 및 루트 컴포넌트 설정
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->TargetArmLength = 150.f;
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->SetupAttachment(RootComponent);

	// 카메라 컴포넌트 생성 및 스프링 암에 붙임
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	MovementSpeed = FVector2D(1000.f, 1000.f);
	ScreenEdgePadding = FVector2D(5.f, 5.f);
	CameraHeightSpeed = 2500.0f;
	ShouldRotate = false;
	RotationSpeed = 100.0f;
	SetActorRotation(FRotator::MakeFromEuler(FVector3d(0, -30, 0)));
}

void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<APlayerController>(GetController());

    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerController is nullptr in ACameraPawn!"));
    }
}

void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto CurrentMovementSpeed = CurrentInputMovementSpeed;
	if (FVector2D MousePosition; CurrentInputMovementSpeed.X == 0 && CurrentInputMovementSpeed.Y == 0 && PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y)) {
		int32 ViewportWidth, ViewportHeight;
		PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);

		const auto X = 
			-(MousePosition.X <= ScreenEdgePadding.X) |
			(MousePosition.X >= ViewportWidth - ScreenEdgePadding.X);

		const auto Y =
			(MousePosition.Y <= ScreenEdgePadding.Y) |
			-(MousePosition.Y >= ViewportHeight - ScreenEdgePadding.Y);

		CurrentMovementSpeed = FVector2D(X, Y);
	}

	//Move the camera
	auto FowardVector = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0);
	FowardVector.Normalize();

	const auto Foward = FowardVector * CurrentMovementSpeed.Y * MovementSpeed.X * DeltaTime;
	const auto Sideways = GetActorRightVector() * CurrentMovementSpeed.X * MovementSpeed.Y * DeltaTime;

	const auto NextLocation = GetActorLocation() + Foward + Sideways;

	FHitResult Hit;
	SetActorLocation(NextLocation, true, &Hit);
}

void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto Subsystem = Cast<APlayerController>(GetController())->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()) {
		Subsystem->AddMappingContext(MappingContext, 0);
	}

	if (const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACameraPawn::MoveTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACameraPawn::MoveCompleted);

		EnhancedInputComponent->BindAction(CameraHeightAction, ETriggerEvent::Triggered, this, &ACameraPawn::CameraHeightTriggered);

		EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Started, this, &ACameraPawn::RotationStarted);
		EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Canceled, this, &ACameraPawn::RotationCompleted);
		EnhancedInputComponent->BindAction(RotationAction, ETriggerEvent::Completed, this, &ACameraPawn::RotationCompleted);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACameraPawn::LookTriggered);
	} 
}

void ACameraPawn::MoveTriggered(const FInputActionValue &Value)
{
	CurrentInputMovementSpeed = Value.Get<FVector2D>();
}

void ACameraPawn::MoveCompleted(const FInputActionValue &Value)
{
	CurrentInputMovementSpeed = FVector2D::Zero();
}

void ACameraPawn::CameraHeightTriggered(const FInputActionValue &Value)
{
	auto Location = GetActorLocation();
	Location.Z += Value.Get<float>() * CameraHeightSpeed * FApp::GetDeltaTime();

	FHitResult Hit;
	SetActorLocation(Location, true, &Hit);
}

void ACameraPawn::RotationStarted(const FInputActionValue &Value)
{
	ShouldRotate = true;
}

void ACameraPawn::RotationCompleted(const FInputActionValue &Value)
{
	ShouldRotate = false;
}

void ACameraPawn::LookTriggered(const FInputActionValue &Value)
{
	const auto& LookVector = Value.Get<FVector2D>();

	if(ShouldRotate) {
		FRotator CurrentRotation = GetActorRotation();
		CurrentRotation.Pitch += RotationSpeed * FApp::GetDeltaTime() * LookVector.Y;
		CurrentRotation.Yaw += RotationSpeed * FApp::GetDeltaTime() * LookVector.X;
		SetActorRotation(CurrentRotation);
	}
}
