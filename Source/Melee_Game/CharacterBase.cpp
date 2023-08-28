// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Melee_Game/CombatComponentPlayer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetWorldLocation(FVector(0.f, 15.f, 60.f));
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	Camera->bUsePawnControlRotation = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.4f;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	CombatComponent = CreateDefaultSubobject<UCombatComponentPlayer>(TEXT("Combat Component"));

	bIsDodging = false;

}




void ACharacterBase::PerformAttack(int AttackIndex)
{
	UAnimMontage* L_AttackMontage = CombatComponent->AttackMontages[AttackIndex];
	if (L_AttackMontage) 
	{
		CombatComponent->bIsAttacking = true;
		PlayAnimMontage(L_AttackMontage);

		CombatComponent->AttackCount++;

		if (CombatComponent->AttackCount > CombatComponent->AttackMontages.Num() - 1) 
		{
			CombatComponent->AttackCount = 0;
		}

	}
}

void ACharacterBase::PerformDodge(int MontageIndex)
{
	if (MontageIndex > CombatComponent->DodgeMontages.Num()-1) 
	{
		MontageIndex = 0;
	}
	else 
	{
		UAnimMontage* L_DodgeMontage = CombatComponent->DodgeMontages[MontageIndex];
		if (L_DodgeMontage)
		{
			bIsDodging = true;
			PlayAnimMontage(L_DodgeMontage);

		}

	}

	


}


void ACharacterBase::LightAttack()
{
	if (CanPerformAttack()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("ATTACKING"));
		UE_LOG(LogTemp, Warning, TEXT("%i"), CombatComponent->AttackCount);

		if (CombatComponent->bIsAttacking) 
		{
			CombatComponent->bAttackSaved = true;
		}
		else 
		{
			PerformAttack(CombatComponent->AttackCount);
		}

	}


	
}



// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	
	
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ACharacterBase::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("LightAttack", EInputEvent::IE_Pressed, this, &ACharacterBase::LightAttack);

	PlayerInputComponent->BindAction("Dodge", EInputEvent::IE_Pressed, this, &ACharacterBase::Dodge);

}

void ACharacterBase::ContinueAttack_Implementation()
{
	CombatComponent->bIsAttacking = false;
	if (CombatComponent->bAttackSaved) 
	{
		CombatComponent->bAttackSaved = false;
		PerformAttack(CombatComponent->AttackCount);
	}

}

void ACharacterBase::ResetAttack_Implementation()
{
	CombatComponent->ResetAttack();
}

FRotator ACharacterBase::GetRotationPlayer_Implementation()
{
	FVector LastInput = FVector(0.001, 0.001, 0.001);

	if (GetCharacterMovement()->GetLastInputVector() != LastInput)
	{
		return UKismetMathLibrary::MakeRotFromX(GetLastMovementInputVector());
	}
	else 
	{
		return GetActorRotation();
	}

}

void ACharacterBase::ResetCombat_Implementation()
{
	CombatComponent->ResetAttack();
	bIsDodging = false;
}

bool ACharacterBase::CanPerformAttack()
{
	return !CombatComponent->bIsAttacking;
}

bool ACharacterBase::CanPerformDodge()
{
	return !CombatComponent->bIsAttacking && !bIsDodging;
}

void ACharacterBase::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0) 
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value); 
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::Dodge()
{
	if (CanPerformDodge()) 
	{
		PerformDodge(0);
	}

}

void ACharacterBase::LookUpAtRate(float Value)
{
	//FOR CONSOLE
}

void ACharacterBase::TurnAtRate(float Value)
{
	//FOR CONSOLE
}


