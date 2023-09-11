// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterClimbSystem.generated.h"



class UCameraComponent;
class USpringArmComponent;
class UCombatComponentPlayer;
class UAnimMontage;
class UInputMappingContext;
class UInputAction;
class UCustomMovementComponent;

UCLASS()
class MELEE_GAME_API ACharacterClimbSystem : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterClimbSystem(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		UCustomMovementComponent* CustomMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs")
		UInputMappingContext* InputContextPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs")
		UInputAction* MovementInput;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs")
		UInputAction* LookingInput;

	void MovementPlayer(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void OnClimbInputStarted(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs")
		UInputAction* ClimbInput;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
		float BaseTurnRate;

	UPROPERTY(EditAnywhere)
		float BaseLookUpRate;



	FORCEINLINE class UCustomMovementComponent* GetCustomMovementComponent() const { return CustomMovementComponent; }
	
};
