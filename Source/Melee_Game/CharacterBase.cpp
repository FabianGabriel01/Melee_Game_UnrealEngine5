// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
//#include "GameFramework/CharacterMovementComponent.h"
#include "Melee_Game/ClimbSystem/Components/CustomMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"
#include "Melee_Game/CombatComponentPlayer.h"
#include "Melee_Game/CollisionComponent_C_Player.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Melee_Game/ClimbSystem/DebugHelpers.h"
////////////Online Subsystem
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"


// Sets default values
ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CustomMovementComponent = Cast<UCustomMovementComponent>(GetCharacterMovement());

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetWorldLocation(FVector(0.f, 15.f, 60.f));
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	Camera->bUsePawnControlRotation = false;

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
	GetCharacterMovement()->JumpZVelocity = 500.f;
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




	////////////////////ONLINE SUBSYSTEM/////////////////////
	//IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	//if (OnlineSubsystem) 
	//{
	//	//At this point yu need a BUILD PACKAGE from the game an got and sign in to Steam
	//	OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	//	if (GEngine) 
	//	{
	//		GEngine->AddOnScreenDebugMessage
	//		(
	//			-1,
	//			15.f,
	//			FColor::Emerald,
	//			FString::Printf(TEXT("Found Subsystem %s"),
	//				*OnlineSubsystem->GetSubsystemName().ToString()
	//			));

	//	}
	//}


}


// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->SetCollisionMeshComponentRight(GetMesh());
	CollisionComponent->SetCollisionMeshComponentLeft(GetMesh());
	CollisionComponent->AddActorToIgnore(GetOwner());
	CollisionComponent->OnHitDispatcher.AddDynamic(this, &ACharacterBase::HIT);


	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputContextPlayer, 0);
		}
	}
	
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

	//PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ACharacterBase::MoveForward);
	//PlayerInputComponent->BindAxis("Move Right / Left", this, &ACharacterBase::MoveRight);
	//PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);


	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("LightAttack", EInputEvent::IE_Pressed, this, &ACharacterBase::LightAttack);
	PlayerInputComponent->BindAction("LightAttack", EInputEvent::IE_Released, this, &ACharacterBase::LightAttackReleased);

	PlayerInputComponent->BindAction("HeavyAttack", EInputEvent::IE_Pressed, this, &ACharacterBase::HeavyAttack);
	PlayerInputComponent->BindAction("HeavyAttack", EInputEvent::IE_Released, this, &ACharacterBase::LightAttack);

	PlayerInputComponent->BindAction("LightAttackKick", EInputEvent::IE_Pressed, this, &ACharacterBase::HeavyKickAttack);
	PlayerInputComponent->BindAction("LightAttackKick", EInputEvent::IE_Released, this, &ACharacterBase::HeavyKickAttackReleased);




	PlayerInputComponent->BindAction("Dodge", EInputEvent::IE_Pressed, this, &ACharacterBase::Dodge);
	//PlayerInputComponent->BindAction("TestingP", EInputEvent::IE_Pressed, this, &ACharacterBase::EnableRagdoll);

	/////////////USING ENHANCED INPUT SYSTEM
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementInput, ETriggerEvent::Triggered, this, &ACharacterBase::MovementPlayer);
		EnhancedInputComponent->BindAction(LookingInput, ETriggerEvent::Triggered, this, &ACharacterBase::LookAround);
		EnhancedInputComponent->BindAction(ClimbInput, ETriggerEvent::Started, this, &ACharacterBase::OnClimbInputStarted);

	}




}

void ACharacterBase::MovementPlayer(const FInputActionValue& Value)
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

void ACharacterBase::LookAround(const FInputActionValue& Value)
{
	FVector2D Look = Value.Get<FVector2D>();
	if (GetController()) 
	{
		AddControllerYawInput(Look.X);
		AddControllerPitchInput(Look.Y);
	}

}

void ACharacterBase::HandleGroundMovementInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);



}

void ACharacterBase::HandleClimbMovementInput(const FInputActionValue& Value)
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

void ACharacterBase::OnClimbInputStarted(const FInputActionValue& Value)
{
	if (!CustomMovementComponent) return;

	if (!CustomMovementComponent->IsClimbing())
	{
		CustomMovementComponent->ToggleClimbing(true);
	}
	else
	{
		CustomMovementComponent->ToggleClimbing(false);

	}
}

void ACharacterBase::PerformAttack(int AttackIndex, ECharacterAction AttackType)
{
	TArray<UAnimMontage*> L_AttackMontagesArray = CombatComponent->GetActionMontages(AttackType);
	UAnimMontage* L_AttackMontage = L_AttackMontagesArray[AttackIndex];

	if (L_AttackMontage) 
	{
		//CombatComponent->bIsAttacking = true;

		StateManagerComponent->SetState(ECharacterState::CS_ATTACKING);
		StateManagerComponent->SetCurrentAction(AttackType);
		PlayAnimMontage(L_AttackMontage);

		CombatComponent->AttackCount++;

		if (CombatComponent->AttackCount > L_AttackMontagesArray.Num() - 1)
		{
			CombatComponent->AttackCount = 0;
		}

	}
}

void ACharacterBase::PerformDodge(int MontageIndex)
{
	//if (!CustomMovementComponent->IsClimbing()) return;
	//CustomMovementComponent->SetMovementMode(EMovementMode::MOVE_Falling);

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
	TrackChargedAttack(HoldKey1);
}

void ACharacterBase::LightAttackReleased()
{
	if (ResetChargedAttack()) 
	{
		bHeavyAttack = false;

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

}

void ACharacterBase::HeavyAttack()
{
	bHeavyAttack = true;
	if (CanPerformAttack() && bHeavyAttack) 
	{
		PerformAttack(CombatComponent->AttackCount, ECharacterAction::CA_UPPERCUT);
	}
}

void ACharacterBase::TrackChargedKickAttack(FKey Key2)
{
	HoldKey2 = Key2;
	GetWorld()->GetTimerManager().SetTimer(TimerPress3, this, &ACharacterBase::ChargedKickAttackTimerEvent, 0.016, true);
	UE_LOG(LogTemp, Warning, TEXT("Track charged Kick Attack"));
}

void ACharacterBase::ChargedKickAttackTimerEvent()
{
	AttackHoldTime = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputKeyTimeDown(HoldKey2);
	bAttackCharged = AttackHoldTime >= 0.18f;
	if (bAttackCharged)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerPress4, FTimerDelegate::CreateLambda([=]()
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerPress3);
			}
		), 0.016, false);
		UE_LOG(LogTemp, Warning, TEXT("Clear Charged Kick Attack"));

		ChargedKickAttackEvent();
	}
}

void ACharacterBase::ChargedKickAttackEvent()
{
	if (CanPerformAttack()) 
	{
		PerformAttack(CombatComponent->AttackCount, ECharacterAction::CA_CHARGEDKICK);
	}
}

bool ACharacterBase::ResetChargedKickAttack()
{
	GetWorld()->GetTimerManager().SetTimer(TimerPress4, FTimerDelegate::CreateLambda([=]()
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerPress3);
		}
	), 0.016, false);
	UE_LOG(LogTemp, Warning, TEXT("Clear Charged Kick Attack"));
	AttackHoldTime = 0;
	if (bAttackCharged)
	{
		bAttackCharged = false;
		bReturnAttackCharged = false;
	}
	else
	{
		bReturnAttackCharged = true;
	}
	return bReturnAttackCharged;
}

void ACharacterBase::HeavyKickAttack()
{
	TrackChargedKickAttack(HoldKey2);
}

void ACharacterBase::HeavyKickAttackReleased()
{
	if (ResetChargedKickAttack()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("HEAVY KICK1"));

		if (StateManagerComponent->GetCurrentState() != ECharacterState::CS_ATTACKING)
		{
			UE_LOG(LogTemp, Warning, TEXT("HEAVY KICK2"));

			if (CanPerformAttack())
			{
				UE_LOG(LogTemp, Warning, TEXT("HEAVY KICK3"));
				PerformAttack(CombatComponent->AttackCount, ECharacterAction::CA_HEAVYKICK);
			}
		}

	}

}

void ACharacterBase::TrackChargedAttack(FKey Key)
{
	HoldKey1 = Key;
	GetWorld()->GetTimerManager().SetTimer(TimerPress1, this, &ACharacterBase::ChargedAttackTimerEvent, 0.016, true);
	UE_LOG(LogTemp, Warning, TEXT("Track charged Attack"));
}

void ACharacterBase::ChargedAttackTimerEvent()
{
	AttackHoldTime =  UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputKeyTimeDown(HoldKey1);
	bAttackCharged = AttackHoldTime >= 0.18f;
	if (bAttackCharged) 
	{
		GetWorld()->GetTimerManager().SetTimer(TimerPress2, FTimerDelegate::CreateLambda([=]()
			{
				GetWorld()->GetTimerManager().ClearTimer(TimerPress1);
			}
		), 0.016, false);
		UE_LOG(LogTemp, Warning, TEXT("Clear Charged Attack"));

		ChargedAttackevent();
	}
}

void ACharacterBase::ChargedAttackevent()
{
	if (CanPerformAttack()) 
	{
		PerformAttack(CombatComponent->AttackCount, ECharacterAction::CA_CHARGED);
	}
}

bool ACharacterBase::ResetChargedAttack()
{
	GetWorld()->GetTimerManager().SetTimer(TimerPress2, FTimerDelegate::CreateLambda([=]()
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerPress1);
		}
	), 0.016, false);
	UE_LOG(LogTemp, Warning, TEXT("Clear Charged Attack"));
	AttackHoldTime = 0;
	if (bAttackCharged) 
	{
		bAttackCharged = false;
		bReturnAttackCharged = false;
	}
	else 
	{
		bReturnAttackCharged = true;
	}
	return bReturnAttackCharged;
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

void ACharacterBase::ApplyHitReaction(AActor* Causer, ECharacterAction AttackTypeH)
{
	switch (AttackTypeH)
	{
	case ECharacterAction::CA_NONE:
		break;
	case ECharacterAction::CA_LIGHT:
		GetCharacterMovement()->AddImpulse(Causer->GetActorForwardVector() * 500.0f, true);
		break;
	case ECharacterAction::CA_UPPERCUT:
		GetCharacterMovement()->AddImpulse(FVector(Causer->GetActorForwardVector().X * 250, Causer->GetActorForwardVector().Y * 250, 1000.0f), true);
		break;
	case ECharacterAction::CA_CHARGED:
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Causer->GetActorLocation()));
		PlayAnimMontage(PerformKnockdownAnim);
		GetCharacterMovement()->AddImpulse(Causer->GetActorForwardVector() * 1500.0f, true);
		GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
		break;
	case ECharacterAction::CA_HEAVYKICK:
		GetCharacterMovement()->AddImpulse(Causer->GetActorForwardVector() * 1000.0f, true);
		break;
	case ECharacterAction::CA_CHARGEDKICK:
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Causer->GetActorLocation()));
		PlayAnimMontage(PerformKnockdownAnim);
		GetCharacterMovement()->AddImpulse(Causer->GetActorForwardVector() * 2000.0f, true);
		GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
		break;
	default:
		break;
	}


	
	TimelineOn();

	bIsDisable = true;

	//PlayHitReactionMontage
	//StateManagerComponent to DISABLE

}

float ACharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	
	UGameplayStatics::PlaySoundAtLocation(this, HitSound, DamageCauser->GetActorLocation());

	ECharacterAction L_Action;

	//UActorComponent* Comp = Causer->GetComponentByClass(UStateManagerComponent_Player::StaticClass());
	switch (DamageCauser->FindComponentByClass<UStateManagerComponent_Player>()->GetCurrentAction())
	{
	case ECharacterAction::CA_LIGHT:
		UE_LOG(LogTemp, Warning, TEXT("LightAttack"));
		L_Action = ECharacterAction::CA_LIGHT;
		break;

	case ECharacterAction::CA_UPPERCUT:
		UE_LOG(LogTemp, Warning, TEXT("Uppercut"));
		L_Action = ECharacterAction::CA_UPPERCUT;
		break;

	case ECharacterAction::CA_CHARGED:
		UE_LOG(LogTemp, Warning, TEXT("ChargedAttack"));
		L_Action = ECharacterAction::CA_CHARGED;
		break;

	case ECharacterAction::CA_HEAVYKICK:
		UE_LOG(LogTemp, Warning, TEXT("HeavyKickAttack"));
		L_Action = ECharacterAction::CA_HEAVYKICK;
		break;

	case ECharacterAction::CA_CHARGEDKICK:
		UE_LOG(LogTemp, Warning, TEXT("ChargedKickAttack"));
		L_Action = ECharacterAction::CA_CHARGEDKICK;
		break;
	default:
		break;
	}
	//Causer->FindComponentByClass<UStateManagerComponent_Player>()->GetCurrentAction();



	ApplyHitReaction(DamageCauser, L_Action);
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

void ACharacterBase::DisableCollision_Implementation()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
}

void ACharacterBase::EnableCollision_Implementation()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

}

bool ACharacterBase::CanPerformAttack()
{
	//return !CombatComponent->bIsAttacking;

	return !StateManagerComponent->IsCurrentStateEqualToAny(StatesToCheckInCanAttack) && !CustomMovementComponent->IsClimbing();

	
}

bool ACharacterBase::CanPerformDodge()
{
	//return !CombatComponent->bIsAttacking && !bIsDodging && !GetCharacterMovement()->IsFalling();

	return !StateManagerComponent->IsCurrentStateEqualToAny(StatesToCheckInCanDodge) && !GetCharacterMovement()->IsFalling() && !CustomMovementComponent->IsClimbing();
}


////////////// ONLINE SUBSYSTEM
void ACharacterBase::OpenLobby()
{
	GetWorld()->ServerTravel("/Game/CombatSystem/Maps/Lobby?listen");
}

void ACharacterBase::CallOpenLevel(const FString& Address)
{
	UGameplayStatics::OpenLevel(GetWorld(), *Address);
}

void ACharacterBase::CallClientTravel(const FString& Address)
{
	APlayerController* PlayerCcontroller = GetGameInstance()->GetFirstLocalPlayerController();
	if (PlayerCcontroller) 
	{
		PlayerCcontroller->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
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


