// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CLIMBINGMECHANIC_API UCustomMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:

	// Climb trace
	TArray<FHitResult> DoMultiCapsuleTraceByObject(FVector Start, FVector End, bool bShowDebug = false);

	void TraceClimbableSurfaces();

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float CapsuleTraceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	float CapsuleTraceHalfHeight = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement|Climbing")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToTrace;
	
};
