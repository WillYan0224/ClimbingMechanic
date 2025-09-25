// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CustomMovementComponent.h"

#include "Kismet/KismetSystemLibrary.h"

void UCustomMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TraceClimbableSurfaces();
}

TArray<FHitResult> UCustomMovementComponent::DoMultiCapsuleTraceByObject(FVector Start,FVector End,	bool bShowDebug)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;
	
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,  Start, End, CapsuleTraceRadius, CapsuleTraceHalfHeight,
		ObjectTypesToTrace, false, TArray<AActor*>(),
		bShowDebug ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None, OutCapsuleTraceHitResults, false
	);
	

	return OutCapsuleTraceHitResults;
}

void UCustomMovementComponent::TraceClimbableSurfaces()
{
	const FVector StartOffset =  UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	DoMultiCapsuleTraceByObject(Start, End, true);
}
