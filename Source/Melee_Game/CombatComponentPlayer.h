// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ECharacterAction.h"
#include "CombatComponentPlayer.generated.h"

class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_GAME_API UCombatComponentPlayer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponentPlayer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	UPROPERTY()
	int AttackCount;

	UPROPERTY()
	bool bAttackSaved;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> DodgeMontages;

	UPROPERTY(EditAnywhere)
		TArray<UAnimMontage*> UppercutMontages;

	UPROPERTY(EditAnywhere)
		TArray<UAnimMontage*> ChargedAttackMontages;

	UFUNCTION()
		TArray<UAnimMontage*> GetActionMontages(ECharacterAction Actions);

	UPROPERTY()
		TArray<UAnimMontage*> ReturnActionsMontages;

	UPROPERTY(EditAnywhere)
		TArray<UAnimMontage*> HeavyKickAttackMontages;

	UPROPERTY(EditAnywhere)
		TArray<UAnimMontage*> ChargedKickMontages;

	UFUNCTION()
	void ResetAttack();



};
