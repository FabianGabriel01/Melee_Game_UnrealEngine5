// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstance_CharacterClimb.generated.h"

class ACharacterClimbSystem;
class UCustomMovementComponent;

/**
 * 
 */
UCLASS()
class MELEE_GAME_API UAnimInstance_CharacterClimb : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY()
		ACharacterClimbSystem* ClimbCharacter;

	UPROPERTY()
		UCustomMovementComponent* CustomMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;
	void GetGroundSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties", meta = (AllowPrivateAccess = "true"))
		float AirSpeed;
	void GetAirSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties", meta = (AllowPrivateAccess = "true"))
		bool bShouldMove;
	void GetbShouldMove();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties", meta = (AllowPrivateAccess = "true"))
		bool bIsFalling;
	void GetbIsFalling();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties", meta = (AllowPrivateAccess = "true"))
		bool bIsClimbing;

	void GetIsClimbing();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties", meta = (AllowPrivateAccess = "true"))
		FVector ClimbVelocity;

	void GetClimbVelocity();


	
};
