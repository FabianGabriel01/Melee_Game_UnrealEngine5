// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ResetCombatC_AN.generated.h"

/**
 * 
 */
UCLASS()
class MELEE_GAME_API UResetCombatC_AN : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
