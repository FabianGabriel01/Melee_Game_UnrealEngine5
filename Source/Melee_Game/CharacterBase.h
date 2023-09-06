// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Combat_CI.h"
#include "CollisionComponent_C_Player.h"
#include "Engine/EngineTypes.h"
#include "CharacterState.h"
#include "StateManagerComponent_Player.h"
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

	UPROPERTY(EditAnywhere)
	UCollisionComponent_C_Player* CollisionComponent;

	UPROPERTY(EditAnywhere)
	UStateManagerComponent_Player* StateManagerComponent;


	UPROPERTY(EditAnywhere)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ParticleHit;

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

	UPROPERTY()
		bool bIsDisable;

	UPROPERTY(EditAnywhere)
		float BaseDamage;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UDamageType> DamageT;

	UFUNCTION(BlueprintCallable)
		void HIT(FHitResult HitResult);

	UFUNCTION()
		void ApplyHitReaction(AActor* Causer, ECharacterAction AttackTypeH);

	UPROPERTY(EditAnywhere)
		float Health;

	bool bHeavyAttack;

	UPROPERTY(EditAnywhere)
	bool bAttackCharged;

	bool bReturnAttackCharged;

	UPROPERTY(EditAnywhere)
		float AttackHoldTime;

	UPROPERTY(EditAnywhere)
		FKey HoldKey1;

	FTimerHandle TimerPress1;
	FTimerHandle TimerPress2;


	TArray<ECharacterState> StatesToCheckInCanAttack;

	TArray<ECharacterState> StatesToCheckInCanDodge;

	UPROPERTY(EditAnywhere)
	UAnimMontage* PerformKnockdownAnim;

	UFUNCTION(BlueprintImplementableEvent)
		void TimelineOn();

	/*UFUNCTION()
		void ApplyPointDamage(AActor* DamagedActor, float Damage, const FVector &HitFromDirection, const FHitResult &HitInfo, AController* EventInstigator, AActor* DamageCauser);*/

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	UFUNCTION()
		void ReceiveDamage(float InDamage, AActor* DamageCauser);

	UFUNCTION()
		void EnableRagdoll(FVector LastHit);

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
	void PerformAttack(int AttackIndex, ECharacterAction AttackType);

	void LightAttack();

	void LightAttackReleased();

	void HeavyAttack();

	void TrackChargedAttack(FKey Key);

	void ChargedAttackTimerEvent();

	void ChargedAttackevent();

	bool ResetChargedAttack();

	UFUNCTION()
	void PerformDodge(int MontageIndex);



};
