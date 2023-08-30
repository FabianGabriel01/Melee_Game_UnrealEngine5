// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionComponent_C_Player.h"

// Sets default values for this component's properties
UCollisionComponent_C_Player::UCollisionComponent_C_Player()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Params.AddIgnoredActor(GetOwner());

	// ...
}


// Called when the game starts
void UCollisionComponent_C_Player::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCollisionComponent_C_Player::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (bCollisionEnabled) 
	{
		CollisionTrace();
	}
}

void UCollisionComponent_C_Player::EnableCollision()
{
	ClearHitActors();
	bCollisionEnabled = true;
}

void UCollisionComponent_C_Player::DisableCollision()
{
	bCollisionEnabled = false;
}

void UCollisionComponent_C_Player::CollisionTrace()
{
	if (GetWorld())
	{

		bool bIsHit = GetWorld()->SweepMultiByChannel
		(
			Hits,
			CollisionMeshComponent->GetSocketLocation(StartSocket),
			CollisionMeshComponent->GetSocketLocation(EndSocket),
			FQuat::Identity,
			ECollisionChannel::ECC_Pawn,
			FCollisionShape::MakeSphere(20.0f),
			Params
		);

		if (bIsHit) 
		{
			for (FHitResult ActorHitted : Hits)
			{
				LastHit = ActorHitted;
				if (!AlreadyHitActors.Contains(LastHit.GetActor()))
				{
					AActor* HitActor = LastHit.GetActor();
					AlreadyHitActors.Add(HitActor);
					OnHitDispatcher.Broadcast(LastHit);

					if (GEngine)
					{
						FString N = FString::Printf(TEXT("RESULT %s"), *ActorHitted.GetActor()->GetName());
						GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, N);
					}

				}

			}

		}


		DrawDebugSphere
		(
			GetWorld(),
			CollisionMeshComponent->GetSocketLocation(StartSocket),
			10.0f,
			12,
			FColor::Blue,
			true,
			-1
		);
	}

}

void UCollisionComponent_C_Player::ClearHitActors()
{
	AlreadyHitActors.Empty();
}

TArray<AActor*> UCollisionComponent_C_Player::GetAlreadyHitActors()
{
	return AlreadyHitActors;
}

bool UCollisionComponent_C_Player::IsCollisionEnable()
{
	return bCollisionEnabled;
}

FHitResult UCollisionComponent_C_Player::GetLastHit()
{
	return LastHit;
}

void UCollisionComponent_C_Player::SetCollisionMeshComponent(UPrimitiveComponent* CollisionMeshComp)
{
	CollisionMeshComponent = CollisionMeshComp;
}

UPrimitiveComponent* UCollisionComponent_C_Player::GetCollsionMeshComponent()
{
	return CollisionMeshComponent;
}

void UCollisionComponent_C_Player::AddActorToIgnore(AActor* ActorToIgnore)
{
	ActorsToIgnore.AddUnique(ActorToIgnore);
}

void UCollisionComponent_C_Player::RemoveActorToIgnore(AActor* RemoveActor)
{
	ActorsToIgnore.Remove(RemoveActor);
}

TArray<AActor*> UCollisionComponent_C_Player::GetActorsToIgnore()
{
	return ActorsToIgnore;
}

