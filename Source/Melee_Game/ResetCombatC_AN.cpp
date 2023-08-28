// Fill out your copyright notice in the Description page of Project Settings.


#include "ResetCombatC_AN.h"
#include "Combat_CI.h"

void UResetCombatC_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ICombat_CI::Execute_ResetCombat(MeshComp->GetOwner());
	
}
