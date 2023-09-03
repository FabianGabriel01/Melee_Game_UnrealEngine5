// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionTraceLeft_C_ANS.h"
#include "CollisionComponent_C_Player.h"

void UCollisionTraceLeft_C_ANS::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	UCollisionComponent_C_Player* Component = MeshComp->GetOwner()->FindComponentByClass<UCollisionComponent_C_Player>();
	Component->EnableCollisionLeft();

}

void UCollisionTraceLeft_C_ANS::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UCollisionComponent_C_Player* Component = MeshComp->GetOwner()->FindComponentByClass<UCollisionComponent_C_Player>();
	Component->DisableCollisionLeft();

}
