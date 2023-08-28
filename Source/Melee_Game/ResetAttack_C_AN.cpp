// Fill out your copyright notice in the Description page of Project Settings.


#include "ResetAttack_C_AN.h"
#include "Combat_CI.h"

void UResetAttack_C_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UE_LOG(LogTemp, Warning, TEXT("ResetAttack from AN"));
	ICombat_CI::Execute_ResetAttack(MeshComp->GetOwner());
}
