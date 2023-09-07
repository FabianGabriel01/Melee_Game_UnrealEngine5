// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponentPlayer.h"

// Sets default values for this component's properties
UCombatComponentPlayer::UCombatComponentPlayer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponentPlayer::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCombatComponentPlayer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<UAnimMontage*> UCombatComponentPlayer::GetActionMontages(ECharacterAction Actions)
{
	switch (Actions)
	{
	case ECharacterAction::CA_NONE:
		ReturnActionsMontages = AttackMontages;
		break;
	case ECharacterAction::CA_LIGHT:
		ReturnActionsMontages = AttackMontages;
		break;
	case ECharacterAction::CA_UPPERCUT:
		ReturnActionsMontages = UppercutMontages;
		break;
	case ECharacterAction::CA_CHARGED:
		ReturnActionsMontages = ChargedAttackMontages;
		break;
	case ECharacterAction::CA_HEAVYKICK:
		ReturnActionsMontages = HeavyKickAttackMontages;
		break;
	case ECharacterAction::CA_CHARGEDKICK:
		ReturnActionsMontages = ChargedKickMontages;
		break;
	default:
		break;
	}
	return ReturnActionsMontages;
}

void UCombatComponentPlayer::ResetAttack()
{
	AttackCount = 0;
	bAttackSaved = false;
}

