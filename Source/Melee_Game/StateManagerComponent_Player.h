// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterState.h"
#include "ECharacterAction.h"
#include "StateManagerComponent_Player.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateBegin, ECharacterState, StateBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateEnd, ECharacterState, StateEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionBegin, ECharacterAction, ActionBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEnd, ECharacterAction, ActionEnd);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_GAME_API UStateManagerComponent_Player : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateManagerComponent_Player();

	void SetState(ECharacterState NewState);

	void SetCurrentAction(ECharacterAction NewAction);

	UFUNCTION(BlueprintPure)
	ECharacterState GetCurrentState();

	UFUNCTION(BlueprintPure)
	ECharacterAction GetCurrentAction();

	UPROPERTY(BlueprintAssignable)
	FOnStateBegin OnStateBeginDispatcher;

	UPROPERTY(BlueprintAssignable)
	FOnStateEnd OnStateEndDispatcher;

	UPROPERTY(BlueprintAssignable)
	FOnActionBegin OnActionBeginDispatcher;

	UPROPERTY(BlueprintAssignable)
	FOnActionEnd OnActionEndDispatcher;

	void ResetState();

	UFUNCTION(BlueprintPure)
	bool IsCurrentStateEqualToAny(TArray<ECharacterState> StatesToCheck);

	UFUNCTION(BlueprintPure)
	bool IsCurrentActionEqualToAny(TArray<ECharacterAction> ActionsToCheck);


private:
	UPROPERTY()
	ECharacterState CurrentState;

	UPROPERTY()
		ECharacterAction CurrentAction;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
