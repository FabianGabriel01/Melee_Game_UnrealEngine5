// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee_Game/ClimbSystem/CharacterClimbSystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
//#include "GameFramework/CharacterMovementComponent.h"
#include "Melee_Game/ClimbSystem/Components/CustomMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Melee_Game/ClimbSystem/DebugHelpers.h"

// Sets default values
ACharacterClimbSystem::ACharacterClimbSystem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);

	CustomMovementComponent = Cast<UCustomMovementComponent>(GetCharacterMovement());


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetWorldLocation(FVector(0.f, 15.f, 60.f));
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->TargetArmLength = 100.f;
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	Camera->bUsePawnControlRotation = false;

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.4f;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

}

// Called when the game starts or when spawned
void ACharacterClimbSystem::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputContextPlayer, 0);
		}
	}
	
}

// Called every frame
void ACharacterClimbSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterClimbSystem::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementInput, ETriggerEvent::Triggered, this, &ACharacterClimbSystem::MovementPlayer);
		EnhancedInputComponent->BindAction(LookingInput, ETriggerEvent::Triggered, this, &ACharacterClimbSystem::LookAround);
		EnhancedInputComponent->BindAction(ClimbInput, ETriggerEvent::Started, this, &ACharacterClimbSystem::OnClimbInputStarted);
	}

}

void ACharacterClimbSystem::MovementPlayer(const FInputActionValue& Value)
{
	if (!CustomMovementComponent) return;

	if (CustomMovementComponent->IsClimbing()) 
	{
		HandleClimbMovementInput(Value);
	}
	else 
	{
		HandleGroundMovementInput(Value);
	}


	
}

void ACharacterClimbSystem::HandleGroundMovementInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ACharacterClimbSystem::HandleClimbMovementInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FVector ForwardDirection = FVector::CrossProduct
	(
		-CustomMovementComponent->GetClimbableSurfacenormal(),
		GetActorRightVector()
		
	);

	const FVector RightDirection = FVector::CrossProduct
	(
		-CustomMovementComponent->GetClimbableSurfacenormal(),
		-GetActorUpVector()
	);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);


}

void ACharacterClimbSystem::LookAround(const FInputActionValue& Value)
{
	FVector2D Look = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(Look.X);
		AddControllerPitchInput(Look.Y);
	}
}

void ACharacterClimbSystem::OnClimbInputStarted(const FInputActionValue& Value)
{
	if (!CustomMovementComponent)
	{
		DEBUG::Print(TEXT("NOT VALID COMPONENT MOVEMENT"));
		return;
	}


	if (!CustomMovementComponent->IsClimbing()) 
	{
		CustomMovementComponent->ToggleClimbing(true);
	}
	else
	{
		CustomMovementComponent->ToggleClimbing(false);

	}
}

