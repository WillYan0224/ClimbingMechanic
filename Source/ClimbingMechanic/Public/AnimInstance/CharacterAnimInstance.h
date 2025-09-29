// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
class AClimbingMechanicCharacter;
class UCustomMovementComponent;
UCLASS()
class CLIMBINGMECHANIC_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	// Essential overrides for native update and initialize
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	
	void GetGroundSpeed();
	void GetAirSpeed();
	void GetShouldMove();
	void GetIsFalling();

	UPROPERTY()
	TObjectPtr<AClimbingMechanicCharacter> ClimbingSystemCharacter;
	UPROPERTY()
	TObjectPtr<UCustomMovementComponent> CustomMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = References, meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = References, meta = (AllowPrivateAccess = "true"))
	float AirSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = References, meta = (AllowPrivateAccess = "true"))
	bool bShouldMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = References, meta = (AllowPrivateAccess = "true"))
	bool bIsFalling;
	
	
};

