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
	FORCEINLINE FVector GetClimbableSurfaceNormal() const { return CurrentClimbableSurfaceNormal; }
	
protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;

private:

	// Climb trace
	TArray<FHitResult> DoMultiCapsuleTraceByObject(FVector Start, FVector End, bool bShowDebug = false, bool bDrawPersistantShapes = false);
	FHitResult DoLineTraceSingleByObject(const FVector& Start,const FVector& End,bool bShowDebugShape = false, bool bDrawPersistantShapes = false);

#pragma region  Core
	bool TraceClimbableSurfaces();
	FHitResult TraceFromEyeHeight(float TraceDistance,float TraceStartOffset = 0.f);
	bool CanStartClimbing();
	void StartClimbing();
	void StopClimbing();
	void PhysClimb(float deltaTime, int32 Iterations);
	TArray<FHitResult> ClimbableSurfacesTracedResults;
	void ProcessClimbableSurfacesInfo();

	// Main Climb movement
	FQuat GetClimbRotation(float DeltaTime);
	void SnapMovementToClimbableSurface(float DeltaTime);
	bool CheckShouldStopClimbing();
	
#pragma endregion
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float CapsuleTraceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float CapsuleTraceHalfHeight = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float MaxBreakClimbDeceleration = 2048.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float MaxClimbSpeed = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float MaxClimbAcceleration = 250.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToTrace;

	FVector CurrentClimbableSurfaceNormal = FVector::ZeroVector;
	FVector CurrentClimbableSurfaceLocation = FVector::ZeroVector;
};


