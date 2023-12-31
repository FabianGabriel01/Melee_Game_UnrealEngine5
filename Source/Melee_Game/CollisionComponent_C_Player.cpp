// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionComponent_C_Player.h"

// Sets default values for this component's properties
UCollisionComponent_C_Player::UCollisionComponent_C_Player()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	// ...
}


// Called when the game starts
void UCollisionComponent_C_Player::BeginPlay()
{
	Super::BeginPlay();
	Params.AddIgnoredActor(GetOwner());
	// ...
	
}


// Called every frame
void UCollisionComponent_C_Player::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (bCollisionEnabledRight)
	{
		CollisionTraceRight();
	}

	if(bCollisionEnabledLeft)
	{
		CollisionTraceLeft();
	}

	if (bCollisionEnableLegRight) 
	{
		CollisionTraceLegRight();
	}

	if (bCollisionEnableLegLeft) 
	{
		CollisionTraceLegLeft();
	}

}

void UCollisionComponent_C_Player::EnableCollisionRight()
{
	ClearHitActors();
	bCollisionEnabledRight = true;
}

void UCollisionComponent_C_Player::EnableCollisionLeft()
{
	ClearHitActors();
	bCollisionEnabledLeft = true;
}

void UCollisionComponent_C_Player::DisableCollisionRight()
{
	bCollisionEnabledRight = false;
}

void UCollisionComponent_C_Player::DisableCollisionLeft()
{
	bCollisionEnabledLeft = false;
}

void UCollisionComponent_C_Player::CollisionTraceRight()
{
	if (GetWorld())
	{

		bool bIsHit = GetWorld()->SweepMultiByChannel
		(
			Hits,
			CollisionMeshComponent->GetSocketLocation(StartSocketRight),
			CollisionMeshComponent->GetSocketLocation(EndSocketRight),
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(20.0f),
			Params
		);

		if (bIsHit) 
		{
			for (const FHitResult ActorHitted : Hits)
			{
				LastHit = ActorHitted;
				if (!AlreadyHitActors.Contains(LastHit.GetActor()))
				{
					AActor* HitActor = LastHit.GetActor();
					AlreadyHitActors.Add(HitActor);
					OnEventDispatcher(LastHit);


					if (GEngine)
					{
						FString N = FString::Printf(TEXT("RESULT %s"), *ActorHitted.GetActor()->GetName());
						GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, N);
					}

				}

			}

		}
	}

}

void UCollisionComponent_C_Player::CollisionTraceLeft()
{
	if (GetWorld())
	{

		bool bIsHit = GetWorld()->SweepMultiByChannel
		(
			Hits,
			CollisionMeshComponent->GetSocketLocation(StartSocketLeft),
			CollisionMeshComponent->GetSocketLocation(EndSocketLeft),
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(10.0f),
			Params
		);

		if (bIsHit)
		{
			for (const FHitResult ActorHitted : Hits)
			{
				LastHit = ActorHitted;
				if (!AlreadyHitActors.Contains(LastHit.GetActor()))
				{
					AActor* HitActor = LastHit.GetActor();
					AlreadyHitActors.Add(HitActor);
					OnEventDispatcher(LastHit);


					if (GEngine)
					{
						FString N = FString::Printf(TEXT("RESULT %s"), *ActorHitted.GetActor()->GetName());
						GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, N);
					}

				}

			}
		}
	}


}

void UCollisionComponent_C_Player::EnableCollisionLegRight()
{
	ClearHitActors();
	bCollisionEnableLegRight = true;

	
}

void UCollisionComponent_C_Player::EnableCollisionLegLeft()
{
	ClearHitActors();
	bCollisionEnableLegLeft = true;

	
}


void UCollisionComponent_C_Player::DisableCollisionLegRight()
{
	bCollisionEnableLegRight = false;
}

void UCollisionComponent_C_Player::DisableCollisionLegLeft()
{
	bCollisionEnableLegLeft = false;
}

void UCollisionComponent_C_Player::CollisionTraceLegRight()
{
	if (GetWorld())
	{

		bool bIsHit = GetWorld()->SweepMultiByChannel
		(
			Hits,
			CollisionMeshComponent->GetSocketLocation(StartSocketLegRight),
			CollisionMeshComponent->GetSocketLocation(EndSocketLegRight),
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(20.0f),
			Params
		);

		if (bIsHit)
		{
			for (const FHitResult ActorHitted : Hits)
			{
				LastHit = ActorHitted;
				if (!AlreadyHitActors.Contains(LastHit.GetActor()))
				{
					AActor* HitActor = LastHit.GetActor();
					AlreadyHitActors.Add(HitActor);
					OnEventDispatcher(LastHit);


					if (GEngine)
					{
						FString N = FString::Printf(TEXT("RESULT %s"), *ActorHitted.GetActor()->GetName());
						GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, N);
					}

				}

			}

		}
	}
}

void UCollisionComponent_C_Player::CollisionTraceLegLeft()
{
	if (GetWorld())
	{

		bool bIsHit = GetWorld()->SweepMultiByChannel
		(
			Hits,
			CollisionMeshComponent->GetSocketLocation(StartSocketLegLeft),
			CollisionMeshComponent->GetSocketLocation(EndSocketLegLeft),
			FQuat::Identity,
			ECollisionChannel::ECC_GameTraceChannel1,
			FCollisionShape::MakeSphere(20.0f),
			Params
		);

		if (bIsHit)
		{
			for (const FHitResult ActorHitted : Hits)
			{
				LastHit = ActorHitted;
				if (!AlreadyHitActors.Contains(LastHit.GetActor()))
				{
					AActor* HitActor = LastHit.GetActor();
					AlreadyHitActors.Add(HitActor);
					OnEventDispatcher(LastHit);


					if (GEngine)
					{
						FString N = FString::Printf(TEXT("RESULT %s"), *ActorHitted.GetActor()->GetName());
						GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Cyan, N);
					}

				}

			}

		}
	}
}

void UCollisionComponent_C_Player::ClearHitActors()
{
	AlreadyHitActors.Empty();
}

void UCollisionComponent_C_Player::OnEventDispatcher(FHitResult HIT)
{
	OnHitDispatcher.Broadcast(HIT);
	UE_LOG(LogTemp, Warning, TEXT("BROADCAST DISPATCHER"));
}

TArray<AActor*> UCollisionComponent_C_Player::GetAlreadyHitActors()
{
	return AlreadyHitActors;
}

bool UCollisionComponent_C_Player::IsCollisionEnableRight()
{
	return bCollisionEnabledRight;
}

bool UCollisionComponent_C_Player::IsCollisionEnableLeft()
{
	return bCollisionEnabledLeft;
}

bool UCollisionComponent_C_Player::IsCollisionEnableLegRight()
{
	return bCollisionEnableLegRight;
}

bool UCollisionComponent_C_Player::IsCollisionEnableLegLeft()
{
	return bCollisionEnableLegLeft;
}

FHitResult UCollisionComponent_C_Player::GetLastHit()
{
	return LastHit;
}

void UCollisionComponent_C_Player::SetCollisionMeshComponentRight(UPrimitiveComponent* CollisionMeshComp)
{
	CollisionMeshComponent = CollisionMeshComp;
}

void UCollisionComponent_C_Player::SetCollisionMeshComponentLeft(UPrimitiveComponent* CollisionMeshComp)
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

