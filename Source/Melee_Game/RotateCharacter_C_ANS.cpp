// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateCharacter_C_ANS.h"
#include "Combat_CI.h"
#include "Kismet/KismetMathLibrary.h"

void URotateCharacter_C_ANS::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	//ICombat_CI::Execute_GetRotationPlayer(MeshComp->GetOwner());
	MeshComp->GetOwner()->SetActorRotation(UKismetMathLibrary::RInterpTo_Constant
	(
		MeshComp->GetOwner()->GetActorRotation(),
		ICombat_CI::Execute_GetRotationPlayer(MeshComp->GetOwner()),
		FrameDeltaTime,
		700.0f
	));

}
