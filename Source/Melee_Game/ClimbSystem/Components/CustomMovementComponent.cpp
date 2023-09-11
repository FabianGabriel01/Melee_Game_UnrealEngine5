// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee_Game/ClimbSystem/Components/CustomMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Melee_Game/ClimbSystem/CharacterClimbSystem.h"
#include "Melee_Game/ClimbSystem/DebugHelpers.h"
#include "Components/CapsuleComponent.h"


void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

#pragma region ClimbTrace
void UCustomMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    if (IsClimbing()) 
    {
        bOrientRotationToMovement = false;
        CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(44.0f);
    }

    if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb) 
    {
        bOrientRotationToMovement = true;
        CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
        StopMovementImmediately();

    }

    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}
void UCustomMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
    if (IsClimbing()) 
    {
        PhysicsClimb(deltaTime, Iterations);
    }

    Super::PhysCustom(deltaTime, Iterations);
}
float UCustomMovementComponent::GetMaxSpeed() const
{
    if (IsClimbing()) 
    {
        return MaxClimbSpeed;
    }
    else 
    {
        return Super::GetMaxSpeed();

    }
}
float UCustomMovementComponent::GetMaxAcceleration() const
{
    if (IsClimbing())
    {
        return MaxClimbAcceleration;
    }
    else
    {
        return Super::GetMaxAcceleration();

    }
}
TArray<FHitResult> UCustomMovementComponent::DoCapsuleMultibyObject(const FVector& Start, const FVector& End, bool bShowDebugSphere, bool bDrawPersistantShaper)
{
    TArray<FHitResult> OutCapulseTraceHitResults;

    EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;
    if (bShowDebugSphere)
    {
        DebugTraceType = EDrawDebugTrace::ForOneFrame;
        if (bDrawPersistantShaper)
        {
            DebugTraceType = EDrawDebugTrace::Persistent;

        }
    }


    UKismetSystemLibrary::CapsuleTraceMultiForObjects
    (
        this,
        Start,
        End,
        CapsuleTraceRadius,
        CapsuleTraceHeight,
        ClimbableSurfaceTypes,
        false,
        TArray<AActor*>(),
        DebugTraceType,
        OutCapulseTraceHitResults,
        false
    );

    return OutCapulseTraceHitResults;
}
FHitResult UCustomMovementComponent::DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebugSphere, bool bDrawPersistantShaper)
{
    FHitResult OutHit;
    EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

    if (bShowDebugSphere) 
    {
        DebugTraceType = EDrawDebugTrace::ForOneFrame;
        if (bDrawPersistantShaper)
        {
            DebugTraceType = EDrawDebugTrace::Persistent;

        }
    }

    UKismetSystemLibrary::LineTraceSingleForObjects
    (
        this,
        Start,
        End,
        ClimbableSurfaceTypes,
        false,
        TArray<AActor*>(),
        DebugTraceType,
        OutHit,
        false
    );

    return OutHit;
}
#pragma endregion


#pragma region ClimbCore


void UCustomMovementComponent::ToggleClimbing(bool bEnableClimb)
{
    if (bEnableClimb) 
    {
        if (CanStartClimbing()) 
        {
            ///Climb State
            DEBUG::Print(TEXT("Can Start Climb"));
            StartClimbing();

        }
        else 
        {
            //EmptyState
            DEBUG::Print(TEXT("Can NOT Start Climb"));

        }
    }
    else 
    {
        ///Stop Climb
        StopClimbing();
    }

}
bool UCustomMovementComponent::IsClimbing() const
{
    return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;
}


bool UCustomMovementComponent::CanStartClimbing()
{
    //Can remove this is i want to keep when is fallinf or not to sart climbing
    if (IsFalling()) return false;

    if (!TraceClimbableSurfaces()) return false;

    if (!TraceFromEyeHeight(100.f).bBlockingHit) return false;

    return true;

}

void UCustomMovementComponent::StartClimbing()
{
    SetMovementMode(MOVE_Custom, ECustomMovementMode::MOVE_Climb);

}

void UCustomMovementComponent::StopClimbing()
{
    SetMovementMode(MOVE_Falling);

}

void UCustomMovementComponent::PhysicsClimb(float deltaTime, int32 Iterations)
{
    if (deltaTime < MIN_TICK_TIME)
    {
        return;
    }
    /* Process all surfaces*/
    TraceClimbableSurfaces();
    ProcessClimbableSurfaceInfo();

    /*Check if we shodl stop climbing*/

    RestorePreAdditiveRootMotionVelocity();

    if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        /*DEFINE MAX CLIMB SPEED*/
        CalcVelocity(deltaTime, 0.f, true, MaxBreakClimbDecelaration);
    }

    ApplyRootMotionToVelocity(deltaTime);

    FVector OldLocation = UpdatedComponent->GetComponentLocation();
    const FVector Adjusted = Velocity * deltaTime;
    FHitResult Hit(1.f);

    /*CLimb Rotation*/
    SafeMoveUpdatedComponent(Adjusted, GetClimbRotation(deltaTime), true, Hit);

    if (Hit.Time < 1.f)
    {
        HandleImpact(Hit, deltaTime, Adjusted);
        SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);

    }

    if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
    }

    /*Snap Movement to surfaces*/
    SnapMovementToClimbableSurfaces(deltaTime);
}

void UCustomMovementComponent::ProcessClimbableSurfaceInfo()
{
    CurrentClimbableSurfaceLocation = FVector::ZeroVector;
    CurrentClimbableSurfaceNormal = FVector::ZeroVector;

    if (ClimbableSurfacesTraceResults.IsEmpty()) return;

    for (const FHitResult& TraceHitResult : ClimbableSurfacesTraceResults) 
    {
        CurrentClimbableSurfaceLocation += TraceHitResult.ImpactPoint;
        CurrentClimbableSurfaceNormal += TraceHitResult.ImpactNormal;
    }

    CurrentClimbableSurfaceLocation /= ClimbableSurfacesTraceResults.Num();
    CurrentClimbableSurfaceNormal = CurrentClimbableSurfaceNormal.GetSafeNormal();
}

FQuat UCustomMovementComponent::GetClimbRotation(float DeltaTime)
{
    const FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();

    if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity()) 
    {
        return CurrentQuat;
    }

    const FQuat TargetQuat = FRotationMatrix::MakeFromX(-CurrentClimbableSurfaceNormal).ToQuat();
    return FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, 5.0f);
}

void UCustomMovementComponent::SnapMovementToClimbableSurfaces(float DeltaTime)
{
    const FVector ComponentForward = UpdatedComponent->GetForwardVector();
    const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
    const FVector ProjectedCharacterToSurface = 
        (CurrentClimbableSurfaceLocation - ComponentLocation).ProjectOnTo(ComponentForward);

    const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedCharacterToSurface.Length();

    UpdatedComponent->MoveComponent(SnapVector * DeltaTime*MaxClimbSpeed, 
        UpdatedComponent->GetComponentQuat(),true);
}



/// <summary>
/// //TRACE FOR CLIMBABLE SURFACE RETURN TRUE If there are indeed valid surfaces, false otherwise
/// </summary>
/// <returns></returns>
bool UCustomMovementComponent::TraceClimbableSurfaces()
{
    const FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.0f;
    const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
    const FVector End = Start + UpdatedComponent->GetForwardVector();

    ClimbableSurfacesTraceResults = DoCapsuleMultibyObject(Start, End, true);
    return !ClimbableSurfacesTraceResults.IsEmpty();
}

FHitResult UCustomMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
    const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
    const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);
    const FVector Start = ComponentLocation + EyeHeightOffset;
    const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

   return DoLineTraceSingleByObject(Start, End);
}

#pragma endregion

