// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomMovementComponent.generated.h"

/**
 * 
 */
class UAnimMontage;
class UAnimInstance;

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type 
	{
		MOVE_Climb UMETA(DisplayName = "ClimbState")

	};
}


UCLASS()
class MELEE_GAME_API UCustomMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

protected:
#pragma region OverridenFunction

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	virtual float GetMaxSpeed() const override;

	virtual float GetMaxAcceleration() const override;



#pragma endregion

private:

#pragma region ClimbTrace
	TArray<FHitResult> DoCapsuleMultibyObject(const FVector& Start, const FVector& End, bool bShowDebugSphere = false, bool bDrawPersistantShaper = false);

	FHitResult DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugSphere = false, bool bDrawPersistantShaper = false);

#pragma endregion

#pragma region ClimbTraceVariables
	TArray<FHitResult> ClimbableSurfacesTraceResults;

	FVector CurrentClimbableSurfaceLocation;
	FVector CurrentClimbableSurfaceNormal;

	UPROPERTY()
		UAnimInstance* OwningPlayerInstance;



#pragma endregion

#pragma region ClimbCore

	bool TraceClimbableSurfaces();

	FHitResult TraceFromEyeHeight(float TraceDistance, float TraceStartOffset = 0.f);

	bool CanStartClimbing();

	void StartClimbing();

	void StopClimbing();

	void PhysicsClimb(float deltaTime, int32 Iterations);

	void ProcessClimbableSurfaceInfo();

	bool CheckShouldStopClimging();


	FQuat GetClimbRotation(float DeltaTime);

	void SnapMovementToClimbableSurfaces(float DeltaTime);

	void PlayClimbMontage(UAnimMontage* MontageToPlay);

	UFUNCTION()
		void onClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted);

#pragma endregion



#pragma region ClimbBPVariables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement : Climbing", meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery> > ClimbableSurfaceTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement : Climbing", meta = (AllowPrivateAccess = "true"))
	float CapsuleTraceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement : Climbing", meta = (AllowPrivateAccess = "true"))
	float CapsuleTraceHeight = 72.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement : Climbing", meta = (AllowPrivateAccess = "true"))
		float MaxBreakClimbDecelaration = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement : Climbing", meta = (AllowPrivateAccess = "true"))
		float MaxClimbSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement : Climbing", meta = (AllowPrivateAccess = "true"))
		float MaxClimbAcceleration = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement : Climbing", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* IdleToClimbMontage;

#pragma endregion


public:
	void ToggleClimbing(bool bEnableClimb);

	bool IsClimbing() const;

	FORCEINLINE FVector GetClimbableSurfacenormal() const { return CurrentClimbableSurfaceNormal;  }

	FVector GetUnrotatedClimbVelocity() const;
	
};
