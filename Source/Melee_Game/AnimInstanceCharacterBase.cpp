// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstanceCharacterBase.h"
#include "Melee_Game/ClimbSystem/Components/CustomMovementComponent.h"
#include "Melee_Game/CharacterBase.h"
#include "Kismet/KismetMathLibrary.h"


void UAnimInstanceCharacterBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ClimbCharacter = Cast<ACharacterBase>(TryGetPawnOwner());
	if (ClimbCharacter)
	{
		CustomMovementComponent = ClimbCharacter->GetCustomMovementComponent();
	}
}

void UAnimInstanceCharacterBase::NativeUpdateAnimation(float DeltaSeconds)
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

void UAnimInstanceCharacterBase::GetGroundSpeed()
{
	GroundSpeed = UKismetMathLibrary::VSizeXY(ClimbCharacter->GetVelocity());

}


void UAnimInstanceCharacterBase::GetAirSpeed()
{
	AirSpeed = ClimbCharacter->GetVelocity().Z;
}

void UAnimInstanceCharacterBase::GetbShouldMove()
{
	if (CustomMovementComponent)
	{
		bShouldMove = CustomMovementComponent->GetCurrentAcceleration().Size() &&
			GroundSpeed > 5.f && !bIsFalling;

	}
}

void UAnimInstanceCharacterBase::GetbIsFalling()
{
	bIsFalling = CustomMovementComponent->IsFalling();
}

void UAnimInstanceCharacterBase::GetIsClimbing()
{
	bIsClimbing = CustomMovementComponent->IsClimbing();
}

void UAnimInstanceCharacterBase::GetClimbVelocity()
{
	ClimbVelocity = CustomMovementComponent->GetUnrotatedClimbVelocity();
}

