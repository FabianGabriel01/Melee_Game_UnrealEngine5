// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Combat_CI.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombat_CI : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MELEE_GAME_API ICombat_CI
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ContinueAttack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ResetAttack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FRotator GetRotationPlayer();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ResetCombat();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisableCollision();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EnableCollision();

};
