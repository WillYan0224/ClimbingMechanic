// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomMovementComponent.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		MOVE_Climb UMETA(DisplayName = "Climb Mode")
	};
}

UCLASS()
class CLIMBINGMECHANIC_API UCustomMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	
	void ToggleClimbing(bool bEnableClimb);
	bool IsClimbing() const;

	
protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

private:

	// Climb trace
	TArray<FHitResult> DoMultiCapsuleTraceByObject(FVector Start, FVector End, bool bShowDebug = false, bool bDrawPersistantShapes = false);
	FHitResult DoLineTraceSingleByObject(const FVector& Start,const FVector& End,bool bShowDebugShape = false, bool bDrawPersistantShapes = false);

	// Core climbing logic
	bool TraceClimbableSurfaces();
	FHitResult TraceFromEyeHeight(float TraceDistance,float TraceStartOffset = 0.f);
	bool CanStartClimbing();
	void StartClimbing();
	void StopClimbing();
	void PhysClimb(float deltaTime, int32 Iterations);
	TArray<FHitResult> ClimbableSurfacesTracedResults;
	void ProcessClimbableSurfacesInfo();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float CapsuleTraceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float CapsuleTraceHalfHeight = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float MaxBreakClimbDeceleration = 2048.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToTrace;

	FVector CurrentClimableSurfaceNormal = FVector::ZeroVector;
	FVector CurrentClimableSurfaceLocation = FVector::ZeroVector;
};
