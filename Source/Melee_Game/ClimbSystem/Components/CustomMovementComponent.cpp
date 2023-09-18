// Fill out your copyright notice in the Description page of Project Settings.


#include "Melee_Game/ClimbSystem/Components/CustomMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Melee_Game/ClimbSystem/CharacterClimbSystem.h"
#include "Melee_Game/ClimbSystem/DebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //CanClimbDownLedge();
}

#pragma region ClimbTrace
void UCustomMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    OwningPlayerInstance = CharacterOwner->GetMesh()->GetAnimInstance();

    if (OwningPlayerInstance)
    {
        OwningPlayerInstance->OnMontageEnded.AddDynamic(this, &UCustomMovementComponent::onClimbMontageEnded);
        OwningPlayerInstance->OnMontageBlendingOut.AddDynamic(this, &UCustomMovementComponent::onClimbMontageEnded);
    }
}
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

        const FRotator DirtyRotation = UpdatedComponent->GetComponentRotation();
        const FRotator CleanStandRotation = FRotator(0.f, DirtyRotation.Yaw, 0.f);
        UpdatedComponent->SetRelativeRotation(CleanStandRotation);

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
FVector UCustomMovementComponent::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const
{
    const bool bIsPlayingRMMontage =
    IsFalling() && OwningPlayerInstance && OwningPlayerInstance->IsAnyMontagePlaying();
    if (bIsPlayingRMMontage) 
    {
        return RootMotionVelocity;
    }
    else 
    {
        return Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity, CurrentVelocity);
    }

    return FVector();
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
            //StartClimbing();
            PlayClimbMontage(IdleToClimbMontage);

        }
        else if(CanClimbDownLedge())
        {
            //EmptyState
            DEBUG::Print(TEXT("CAN CLIMB DOWN"));
            PlayClimbMontage(ClimbDownMontage);

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

bool UCustomMovementComponent::CanClimbDownLedge()
{
    if (IsFalling()) return false;

    const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
    const FVector ComponentForward = UpdatedComponent->GetForwardVector();
    const FVector DownVector = -UpdatedComponent->GetUpVector();


    const FVector WalkableSurfaceTraceStart = ComponentLocation + ComponentForward * ClimbDownWalkableSurfaceTraceOffset;
    const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart + DownVector * 100.f;

    FHitResult WalkableSurfacehit = DoLineTraceSingleByObject(WalkableSurfaceTraceStart, WalkableSurfaceTraceEnd);

    const FVector LedgeTraceStart = WalkableSurfacehit.TraceStart + ComponentForward * ClimbDownTraceOffset;
    const FVector LedgeTraceEnd = LedgeTraceStart + DownVector * 200;
    
    FHitResult LedgeTraceHhit = DoLineTraceSingleByObject(LedgeTraceStart, LedgeTraceEnd);

    if (WalkableSurfacehit.bBlockingHit && !LedgeTraceHhit.bBlockingHit) 
    {
        return true;
    }

    return false;
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
    if (CheckShouldStopClimging() || CheckHasReachedFloor()) 
    {
        StopClimbing();
    }

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

    if(CheckHasReachedLedge())
    {
        DEBUG::Print(TEXT("LEDGED REACHEAD"));
        PlayClimbMontage(ClimbToTopMontage);
    }
    else 
    {
        DEBUG::Print(TEXT("LEDGED NOT REACHED"));

    }
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

bool UCustomMovementComponent::CheckShouldStopClimging()
{
    if (ClimbableSurfacesTraceResults.IsEmpty()) return true;

    const float DotResult = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::UpVector);
    const float DegreeDiff = FMath::RadiansToDegrees(FMath::Acos(DotResult));

    if (DegreeDiff <= 60.f) return true;
    return false;
}

bool UCustomMovementComponent::CheckHasReachedFloor()
{
    const FVector DownVector = -UpdatedComponent->GetUpVector();
    const FVector StartOffset = DownVector * 50.f;
    const FVector Start = UpdatedComponent->GetComponentLocation();
    FVector End = Start + DownVector;

    TArray<FHitResult> PossibleFloorHits = DoCapsuleMultibyObject(Start, End);

    if (PossibleFloorHits.IsEmpty()) return false;

    for (const FHitResult& PossibleFloorHit : PossibleFloorHits) 
    {
        const bool bFloorReached =
        FVector::Parallel(-PossibleFloorHit.ImpactNormal, FVector::UpVector) &&
            GetUnrotatedClimbVelocity().Z < -10.f;


        if (bFloorReached) 
        {
            return true;
        }

    }

    return false;
}

bool UCustomMovementComponent::CheckHasReachedLedge()
{
    FHitResult LedgeHit = TraceFromEyeHeight(100.f, 50.f);
    if (!LedgeHit.bBlockingHit) 
    {
        const FVector WalkableSurfaceTraceStart = LedgeHit.TraceEnd;
        const FVector DownVector = -UpdatedComponent->GetUpVector();
        const FVector WalkableSurfaceEnd = WalkableSurfaceTraceStart + DownVector * 100.f;

        FHitResult WakbableSurfaceHitResult =
        DoLineTraceSingleByObject(WalkableSurfaceTraceStart, WalkableSurfaceEnd);

        if (WakbableSurfaceHitResult.bBlockingHit && GetUnrotatedClimbVelocity().Z > 10.f) 
        {
            return true;
        }

    }

    //const FVector DownVector = UpdatedComponent->GetUpVector();
    //const FVector StartOffset = DownVector * 100.f;
    //const FVector Start = UpdatedComponent->GetComponentLocation();
    //FVector End = Start + DownVector;

    //FHitResult LedgedHit = DoLineTraceSingleByObject(Start, End, true, true);

    ////if(LedgedHit.TraceEnd)
    ////if(LedgedHit.TraceEnd)

    return false;
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


void UCustomMovementComponent::PlayClimbMontage(UAnimMontage* MontageToPlay)
{
    if (!MontageToPlay) return;
    if (!OwningPlayerInstance) return;
    if (OwningPlayerInstance->IsAnyMontagePlaying()) return;

    OwningPlayerInstance->Montage_Play(MontageToPlay);
}

void UCustomMovementComponent::onClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    //DEBUG::Print(TEXT("CLIMB MONTAGES ENDED"));
    if (Montage == IdleToClimbMontage || Montage == ClimbDownMontage) 
    {
        StartClimbing();
        StopMovementImmediately();
    }
    if(Montage == ClimbToTopMontage)
    {
        SetMovementMode(MOVE_Walking);
    }
}


FVector UCustomMovementComponent::GetUnrotatedClimbVelocity() const
{
    return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}



#pragma endregion

