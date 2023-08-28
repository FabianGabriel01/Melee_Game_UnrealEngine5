// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ContinueAttack_C_AN.generated.h"

/**
 * 
 */
UCLASS()
class MELEE_GAME_API UContinueAttack_C_AN : public UAnimNotify
{
	GENERATED_BODY()

public:
	//bool Received_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

};
