// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee_Game/ClimbSystem/AnimInstance_CharacterClimb.h"
#include "Melee_Game/ClimbSystem/CharacterClimbSystem.h"
#include "Melee_Game/ClimbSystem/Components/CustomMovementComponent.h"
#include "Melee_Game/CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"


void UAnimInstance_CharacterClimb::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ClimbCharacter = Cast<ACharacterClimbSystem>(TryGetPawnOwner());
	if (ClimbCharacter) 
	{
		CustomMovementComponent = ClimbCharacter->GetCustomMovementComponent();
	}
}

void UAnimInstance_CharacterClimb::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!ClimbCharacter || !CustomMovementComponent) return;

	GetGroundSpeed();
	GetAirSpeed();
	GetbShouldMove();
	GetbIsFalling();
	GetIsClimbing();
	GetClimbVelocity();
}

void UAnimInstance_CharacterClimb::GetGroundSpeed()
{
	GroundSpeed = UKismetMathLibrary::VSizeXY(ClimbCharacter->GetVelocity());
	
}


void UAnimInstance_CharacterClimb::GetAirSpeed()
{
	AirSpeed = ClimbCharacter->GetVelocity().Z;
}

void UAnimInstance_CharacterClimb::GetbShouldMove()
{
	if (CustomMovementComponent) 
	{
		bShouldMove = CustomMovementComponent->GetCurrentAcceleration().Size() &&
		GroundSpeed> 5.f && !bIsFalling;

	}
}

void UAnimInstance_CharacterClimb::GetbIsFalling()
{
	bIsFalling = CustomMovementComponent->IsFalling();
}

void UAnimInstance_CharacterClimb::GetIsClimbing()
{
	bIsClimbing = CustomMovementComponent->IsClimbing();
}

void UAnimInstance_CharacterClimb::GetClimbVelocity()
{
	ClimbVelocity = CustomMovementComponent->GetUnrotatedClimbVelocity();
}
