// Fill out your copyright notice in the Description page of Project Settings.


#include "StateManagerComponent_Player.h"

// Sets default values for this component's properties
UStateManagerComponent_Player::UStateManagerComponent_Player()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UStateManagerComponent_Player::SetState(ECharacterState NewState)
{
	if (NewState != CurrentState) 
	{
		OnStateEndDispatcher.Broadcast(CurrentState);
		CurrentState = NewState;
		OnStateBeginDispatcher.Broadcast(CurrentState);

	}

}

void UStateManagerComponent_Player::SetCurrentAction(ECharacterAction NewAction)
{
	if (NewAction != CurrentAction) 
	{
		OnActionEndDispatcher.Broadcast(CurrentAction);
		CurrentAction = NewAction;
		OnActionBeginDispatcher.Broadcast(CurrentAction);
	}

}

ECharacterState UStateManagerComponent_Player::GetCurrentState()
{
	return CurrentState;
}

ECharacterAction UStateManagerComponent_Player::GetCurrentAction()
{
	return CurrentAction;
}

void UStateManagerComponent_Player::ResetState()
{
	SetState(ECharacterState::CS_NONE);
}

bool UStateManagerComponent_Player::IsCurrentStateEqualToAny(TArray<ECharacterState> StatesToCheck)
{
	return StatesToCheck.Contains(CurrentState);
}

bool UStateManagerComponent_Player::IsCurrentActionEqualToAny(TArray<ECharacterAction> ActionsToCheck)
{
	return ActionsToCheck.Contains(CurrentAction);
}


// Called when the game starts
void UStateManagerComponent_Player::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStateManagerComponent_Player::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

