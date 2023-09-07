// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CollisionComponent_C_Player.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHit, FHitResult, HitResult);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_GAME_API UCollisionComponent_C_Player : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCollisionComponent_C_Player();

	UPROPERTY(BlueprintAssignable)
	FOnHit OnHitDispatcher;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
		TArray<FHitResult> Hits;
	


	//////RIGHT HAND
	UPROPERTY(EditAnywhere)
		FName StartSocketRight;


	UPROPERTY(EditAnywhere)
		FName EndSocketRight;


	///LEFT HAND
	UPROPERTY(EditAnywhere)
		FName StartSocketLeft;

	UPROPERTY(EditAnywhere)
		FName EndSocketLeft;

	///LEGS
	UPROPERTY(EditAnywhere)
		FName StartSocketLegRight;

	UPROPERTY(EditAnywhere)
		FName EndSocketLegRight;

	UPROPERTY(EditAnywhere)
		FName StartSocketLegLeft;

	UPROPERTY(EditAnywhere)
		FName EndSocketLegLeft;


	///FUNCS
	UFUNCTION()
	void EnableCollisionRight();

	UFUNCTION()
	void EnableCollisionLeft();

	UFUNCTION()
	void DisableCollisionRight();

	UFUNCTION()
	void DisableCollisionLeft();

	UFUNCTION()
	void CollisionTraceRight();

	UFUNCTION()
	void CollisionTraceLeft();

	UFUNCTION()
	void EnableCollisionLegRight();

	UFUNCTION()
	void EnableCollisionLegLeft();
	
	UFUNCTION()
	void DisableCollisionLegRight();

	UFUNCTION()
	void DisableCollisionLegLeft();


	UFUNCTION()
	void CollisionTraceLegRight();

	UFUNCTION()
	void CollisionTraceLegLeft();




	UFUNCTION()
	void ClearHitActors();

	UFUNCTION()
		void OnEventDispatcher(FHitResult LastHit);

	UFUNCTION(BlueprintPure)
	TArray<AActor*> GetAlreadyHitActors();



	UFUNCTION(BlueprintPure)
		bool IsCollisionEnableRight();

	UFUNCTION(BlueprintPure)
		bool IsCollisionEnableLeft();

	UFUNCTION(BlueprintPure)
		bool IsCollisionEnableLegRight();

	UFUNCTION(BlueprintPure)
		bool IsCollisionEnableLegLeft();


	UFUNCTION(BlueprintPure)
		FHitResult GetLastHit();

	UFUNCTION(BlueprintCallable)
		void SetCollisionMeshComponentRight(UPrimitiveComponent* CollisionMeshComp);

	UFUNCTION(BlueprintCallable)
		void SetCollisionMeshComponentLeft(UPrimitiveComponent* CollisionMeshComp);

	UFUNCTION(BlueprintPure)
		UPrimitiveComponent* GetCollsionMeshComponent();

	UFUNCTION()
		void AddActorToIgnore(AActor* ActorToIgnore);

	UFUNCTION()
		void RemoveActorToIgnore(AActor* RemoveActor);

	UFUNCTION(BlueprintPure)
		TArray<AActor*> GetActorsToIgnore();


private:
	UPROPERTY()
		TArray<AActor*> AlreadyHitActors;

	UPROPERTY()
		bool bCollisionEnabledRight;

	UPROPERTY()
		bool bCollisionEnabledLeft;

	UPROPERTY()
		bool bCollisionEnableLegRight;

	UPROPERTY()
		bool bCollisionEnableLegLeft;


	UPROPERTY()
		FHitResult LastHit;

	UPROPERTY()
		UPrimitiveComponent* CollisionMeshComponent;

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	FCollisionQueryParams Params;


		
};
