


// Fill out your copyright notice in the Description page of Project Settings.


#include "EnableDisableCollision_C_ANS.h"
#include "CharacterBase.h"

void UEnableDisableCollision_C_ANS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ICombat_CI::Execute_DisableCollision(MeshComp->GetOwner());
}

void UEnableDisableCollision_C_ANS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ICombat_CI::Execute_EnableCollision(MeshComp->GetOwner());
}
