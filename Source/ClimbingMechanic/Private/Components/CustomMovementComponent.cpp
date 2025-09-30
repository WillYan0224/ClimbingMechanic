// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CustomMovementComponent.h"
#include "ClimbingMechanicCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DebugHelper.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UCustomMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningPlayerAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

	if (OwningPlayerAnimInstance)
	{
		OwningPlayerAnimInstance->OnMontageEnded.AddDynamic(this, &UCustomMovementComponent::OnMontageEnded);
		OwningPlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this, &UCustomMovementComponent::OnMontageEnded);
	}
}

void UCustomMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TraceClimbableSurfaces();
	// TraceFromEyeHeight(200.f,.0f);
}

void UCustomMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	
	if(IsClimbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
	}
	if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::MOVE_Climb)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(96.f);

		StopMovementImmediately();
	}
}

void UCustomMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	if (IsClimbing())
	{
		PhysClimb(deltaTime, Iterations);
	}
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

void UCustomMovementComponent::ToggleClimbing(bool bEnableClimb)
{
	if (bEnableClimb)
	{
		if(CanStartClimbing())
		{
			Debug::Print(TEXT("climbing"));
			PlayClimbMontage(IdleToClimbMontage);
		}
		else
		{
			Debug::Print(TEXT("cannot climbing"));
		}
	}
	else
	{
		StopClimbing();
	}
}

bool UCustomMovementComponent::CanStartClimbing()
{
	if(IsFalling()) return false;
	if(!TraceClimbableSurfaces()) return false;
	if(!TraceFromEyeHeight(100.f).bBlockingHit) return false;

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

void UCustomMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	TraceClimbableSurfaces();
	ProcessClimbableSurfacesInfo();
	
	if (CheckShouldStopClimbing() || CheckHasReachedFloor())
	{
		StopClimbing();
	}
		
	RestorePreAdditiveRootMotionVelocity();

	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		CalcVelocity(deltaTime, .0f, true, MaxBreakClimbDeceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);
	
	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, GetClimbRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	SnapMovementToClimbableSurface(deltaTime);
	if (CheckHasReachedLedge())
	{
		Debug::Print(TEXT("ledge reached"), FColor::Green, 2.f, 1.f);
	}
	else
	{
		Debug::Print(TEXT("no ledge"), FColor::Red, 2.f, 1.f);
	}
}

void UCustomMovementComponent::ProcessClimbableSurfacesInfo()
{
	CurrentClimbableSurfaceLocation = FVector::ZeroVector;
	CurrentClimbableSurfaceNormal = FVector::ZeroVector;

	if (ClimbableSurfacesTracedResults.IsEmpty()) return;
	for (const FHitResult& HitResult : ClimbableSurfacesTracedResults)
	{
		if (HitResult.bBlockingHit)
		{
			CurrentClimbableSurfaceLocation += HitResult.ImpactPoint;
			CurrentClimbableSurfaceNormal += HitResult.ImpactNormal;
		}
	}
	CurrentClimbableSurfaceLocation /= ClimbableSurfacesTracedResults.Num();
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
	return FMath::QInterpTo(CurrentQuat, TargetQuat, DeltaTime, 7.f);
	
}

void UCustomMovementComponent::SnapMovementToClimbableSurface(float DeltaTime)
{
	const FVector ComponentForward = UpdatedComponent->GetForwardVector();
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();

	const FVector ProjectedCharacterToSurface = (CurrentClimbableSurfaceLocation - ComponentLocation).ProjectOnTo(ComponentForward);
	const FVector SnapVector = -CurrentClimbableSurfaceNormal * ProjectedCharacterToSurface.Length();

	UpdatedComponent->MoveComponent(SnapVector * DeltaTime * MaxClimbSpeed, UpdatedComponent->GetComponentQuat(), true);
	
}

bool UCustomMovementComponent::CheckShouldStopClimbing()
{
	if (ClimbableSurfacesTracedResults.IsEmpty()) return true;

	const float DotResult = FVector::DotProduct(CurrentClimbableSurfaceNormal, FVector::UpVector);
	const float ThetaAngle = FMath::RadiansToDegrees(FMath::Acos(DotResult));

	if (ThetaAngle <= 60.f && ThetaAngle >= -60.f)
	{
		return true;
	}
	
	return  false;
}

bool UCustomMovementComponent::CheckHasReachedFloor()
{
	const FVector DownVector = -UpdatedComponent->GetUpVector();
	const FVector Start = UpdatedComponent->GetComponentLocation() + DownVector * 50.f;
	const FVector End = Start + DownVector;

	TArray<FHitResult> FloorTraceResults = DoMultiCapsuleTraceByObject(Start, End);

	if (FloorTraceResults.IsEmpty()) return false;
	
	for (const FHitResult& HitResult : FloorTraceResults)
	{
		const bool bFloorReached = 	FVector::Parallel(-HitResult.ImpactNormal, FVector::UpVector) && GetUnrotatedClimbVelocity().Z < -10.f;
		if (bFloorReached)
		{
			return true;
		}
	}
	return false;
}

bool UCustomMovementComponent::CheckHasReachedLedge()
{
	// 2-times trace UP -> Front -> Down
	// Trace above eye height to see if there is a ledge
	FHitResult LedgeHitResult =	TraceFromEyeHeight(100.f, 50.f);
	// First trace forward to see if there is a wall
	// No Hit = Possible ledge
	if (!LedgeHitResult.bBlockingHit)
	{
		const FVector WalkableSurfaceTraceStartPt = LedgeHitResult.TraceEnd;
		const FVector DownVector = -UpdatedComponent->GetUpVector();
		const FVector WalkableSurfaceTraceEndPt = WalkableSurfaceTraceStartPt + DownVector * 150.f;
		// Second trace down to see if there is a walkable surface
		FHitResult WalkableSurfaceHitResult = DoLineTraceSingleByObject(WalkableSurfaceTraceStartPt, WalkableSurfaceTraceEndPt, true);
		if (WalkableSurfaceHitResult.bBlockingHit && GetUnrotatedClimbVelocity().Z > 10.f)
		{
			return true;
		}
	}
	return false;
}

bool UCustomMovementComponent::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::MOVE_Climb;
}

TArray<FHitResult> UCustomMovementComponent::DoMultiCapsuleTraceByObject(FVector Start,FVector End,	bool bShowDebug, bool bDrawPersistantShapes)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;
	
	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if(bShowDebug)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if(bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		CapsuleTraceRadius,
		CapsuleTraceHalfHeight,
		ObjectTypesToTrace,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutCapsuleTraceHitResults,
		false
	);


	return OutCapsuleTraceHitResults;
}

FHitResult UCustomMovementComponent::DoLineTraceSingleByObject(const FVector& Start, const FVector& End, bool bShowDebug, bool bDrawPersistantShapes)
{
	FHitResult OutLineTraceHitResult;
	EDrawDebugTrace::Type DebugTraceType = EDrawDebugTrace::None;

	if(bShowDebug)
	{
		DebugTraceType = EDrawDebugTrace::ForOneFrame;

		if(bDrawPersistantShapes)
		{
			DebugTraceType = EDrawDebugTrace::Persistent;
		}
	}
	
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ObjectTypesToTrace,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutLineTraceHitResult,
		false
	);

	return OutLineTraceHitResult;
}

bool UCustomMovementComponent::TraceClimbableSurfaces()
{
	const FVector StartOffset =  UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	ClimbableSurfacesTracedResults = DoMultiCapsuleTraceByObject(Start, End, true);

	return !ClimbableSurfacesTracedResults.IsEmpty();
}

FHitResult UCustomMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);
	
	const FVector Start = ComponentLocation + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;

	return DoLineTraceSingleByObject(Start,End, true);
}

FVector UCustomMovementComponent::GetUnrotatedClimbVelocity() const
{
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(), Velocity);
}

void UCustomMovementComponent::PlayClimbMontage(UAnimMontage* MontageToPlay)
{
	if (!MontageToPlay) return;
	if (!OwningPlayerAnimInstance) return;
	if (OwningPlayerAnimInstance->IsAnyMontagePlaying()) return;

	OwningPlayerAnimInstance->Montage_Play(MontageToPlay);
}

void UCustomMovementComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == IdleToClimbMontage)
	{
		StartClimbing();
	}
}
