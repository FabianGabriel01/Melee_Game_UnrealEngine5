// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionTraceLegRight_C_ANS.h"
#include "CollisionComponent_C_Player.h"

void UCollisionTraceLegRight_C_ANS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	UCollisionComponent_C_Player* Component = MeshComp->GetOwner()->FindComponentByClass<UCollisionComponent_C_Player>();
	Component->EnableCollisionLegRight();
}

void UCollisionTraceLegRight_C_ANS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UCollisionComponent_C_Player* Component = MeshComp->GetOwner()->FindComponentByClass<UCollisionComponent_C_Player>();
	Component->DisableCollisionLegRight();
}
