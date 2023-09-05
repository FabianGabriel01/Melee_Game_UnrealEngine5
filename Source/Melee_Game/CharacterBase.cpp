// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Melee_Game/CombatComponentPlayer.h"
#include "Melee_Game/CollisionComponent_C_Player.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"


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
	CollisionComponent = CreateDefaultSubobject<UCollisionComponent_C_Player>(TEXT("Collision Component"));
	StateManagerComponent = CreateDefaultSubobject<UStateManagerComponent_Player>(TEXT("StatemanagerComponent"));

	bIsDodging = false;
	BaseDamage = 20.0f;

	Health = 100.0f;

	StatesToCheckInCanAttack = 
	{
		ECharacterState::CS_ATTACKING,
		ECharacterState::CS_DEATH,
		ECharacterState::CS_DODGING,
		ECharacterState::CS_DISABLE
	};

	StatesToCheckInCanDodge =
	{
		ECharacterState::CS_DEATH,
		ECharacterState::CS_DODGING,
		ECharacterState::CS_DISABLE
	};

}




void ACharacterBase::PerformAttack(int AttackIndex, ECharacterAction AttackType)
{
	UAnimMontage* L_AttackMontage = CombatComponent->AttackMontages[AttackIndex];
	if (L_AttackMontage) 
	{
		//CombatComponent->bIsAttacking = true;

		StateManagerComponent->SetState(ECharacterState::CS_ATTACKING);
		StateManagerComponent->SetCurrentAction(AttackType);
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
			//bIsDodging = true;
			StateManagerComponent->SetState(ECharacterState::CS_DODGING);

			PlayAnimMontage(L_DodgeMontage);

		}

	}

	


}


void ACharacterBase::LightAttack()
{
	if (StateManagerComponent->GetCurrentState() == ECharacterState::CS_ATTACKING)
	{
		CombatComponent->bAttackSaved = true;
	}
	else 
	{
		if (CanPerformAttack())
		{
			// IF
			//CombatComponent->bIsAttacking 
			PerformAttack(CombatComponent->AttackCount, ECharacterAction::CA_LIGHT);
		}
	}
}



// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->SetCollisionMeshComponentRight(GetMesh());
	CollisionComponent->SetCollisionMeshComponentLeft(GetMesh());
	CollisionComponent->AddActorToIgnore(GetOwner());
	CollisionComponent->OnHitDispatcher.AddDynamic(this, &ACharacterBase::HIT);
	
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

	//PlayerInputComponent->BindAction("TestingP", EInputEvent::IE_Pressed, this, &ACharacterBase::EnableRagdoll);

}

void ACharacterBase::HIT(FHitResult HitResult)
{
	UGameplayStatics::ApplyPointDamage
	(
		HitResult.GetActor(),
		BaseDamage,
		GetOwner()->GetActorForwardVector(),
		HitResult, 
		GetInstigatorController(),
		this,
		DamageT
	);

	UGameplayStatics::SpawnEmitterAtLocation(this, ParticleHit, HitResult.ImpactPoint);
}

void ACharacterBase::ApplyHitReaction(AActor* Causer)
{
	GetCharacterMovement()->AddImpulse(Causer->GetActorForwardVector() * 25000.0f, true);
	bIsDisable = true;

	//PlayHitReactionMontage
	//StateManagerComponent to DISABLE

}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UGameplayStatics::PlaySoundAtLocation(this, HitSound, DamageCauser->GetActorLocation());
	ApplyHitReaction(DamageCauser);
	ReceiveDamage(DamageAmount, DamageCauser);

	return 0.0f;
}

void ACharacterBase::ReceiveDamage(float InDamage, AActor* DamageCauser)
{

	Health = UKismetMathLibrary::Clamp(Health- InDamage, 0, 100.0f);
	UE_LOG(LogTemp, Warning, TEXT("%f"), Health);

	if (Health <= 0.0f) 
	{
		EnableRagdoll(DamageCauser->GetActorForwardVector());
	}

}

void ACharacterBase::EnableRagdoll(FVector LastHit)
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

	
	CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("pelvis"));
	CameraBoom->bDoCollisionTest = false;

	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(FName("pelvis"), 1.0);
	GetMesh()->SetPhysicsLinearVelocity(LastHit * 10000.0f, false, FName("pelvis"));

	StateManagerComponent->SetState(ECharacterState::CS_DEATH);
	SetLifeSpan(2.0f);
}

void ACharacterBase::ContinueAttack_Implementation()
{
	//CombatComponent->bIsAttacking = false;
	if (CombatComponent->bAttackSaved) 
	{
		CombatComponent->bAttackSaved = false;
		if (StateManagerComponent->GetCurrentState() == ECharacterState::CS_ATTACKING) 
		{
			StateManagerComponent->SetState(ECharacterState::CS_NONE);
			PerformAttack(CombatComponent->AttackCount, ECharacterAction::CA_LIGHT);
		}
		else 
		{
			PerformAttack(CombatComponent->AttackCount, ECharacterAction::CA_LIGHT);

		}
	}

}

void ACharacterBase::ResetAttack_Implementation()
{
	CombatComponent->ResetAttack();
	bIsDisable = false;
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
	StateManagerComponent->ResetState();
	StateManagerComponent->SetCurrentAction(ECharacterAction::CA_NONE);
	//bIsDodging = false;
}

bool ACharacterBase::CanPerformAttack()
{
	//return !CombatComponent->bIsAttacking;

	return !StateManagerComponent->IsCurrentStateEqualToAny(StatesToCheckInCanAttack);

	
}

bool ACharacterBase::CanPerformDodge()
{
	//return !CombatComponent->bIsAttacking && !bIsDodging && !GetCharacterMovement()->IsFalling();

	return !StateManagerComponent->IsCurrentStateEqualToAny(StatesToCheckInCanDodge) && !GetCharacterMovement()->IsFalling();
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


