// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Combat_CI.h"
#include "CharacterBase.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCombatComponentPlayer;

UCLASS()
class MELEE_GAME_API ACharacterBase : public ACharacter, public ICombat_CI
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere)
	UCombatComponentPlayer* CombatComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
		float BaseTurnRate;

	UPROPERTY(EditAnywhere)
		float BaseLookUpRate;

	UPROPERTY()
		int AttackingFunct = 0;

	UPROPERTY()
		bool bIsDodging;


	///INTERFACES
	void ContinueAttack_Implementation() override;

	void ResetAttack_Implementation() override;

	FRotator GetRotationPlayer_Implementation() override;

	void ResetCombat_Implementation() override;

	//FUNCS
	bool CanPerformAttack();

	bool CanPerformDodge();


private:
	//MOVEMENT
	void MoveForward(float Value);

	void MoveRight(float Value);

	void Dodge();

	//VIEW
	void LookUpAtRate(float Value);

	void TurnAtRate(float Value);

	//Keys
	UFUNCTION()
	void PerformAttack(int AttackIndex);

	void LightAttack();

	UFUNCTION()
	void PerformDodge(int MontageIndex);

};
