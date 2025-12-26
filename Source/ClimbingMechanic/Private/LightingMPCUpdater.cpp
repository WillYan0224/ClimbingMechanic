#include "LightingMPCUpdater.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"

ALightingMPCUpdater::ALightingMPCUpdater()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALightingMPCUpdater::BeginPlay()
{
	Super::BeginPlay();
			
	//  KeyLightActor slot selection default or not
	if (!KeyLightActor)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), Found);
		if (Found.Num() > 0)
		{
			KeyLightActor = Cast<ADirectionalLight>(Found[0]);
		}
	}
	
	if (!LightingMPC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LightingMPCUpdater] LightingMPC not set."));
		return;
	}

	MPCInstance = GetWorld()->GetParameterCollectionInstance(LightingMPC);
	if (!MPCInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LightingMPCUpdater] Failed to get MPC instance."));
		return;
	}

	UpdateMPCOnce();
}

void ALightingMPCUpdater::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bTickUpdate)
	{
		UpdateMPCOnce();
	}
}

void ALightingMPCUpdater::UpdateMPCOnce()
{
	if (!MPCInstance || !KeyLightActor) return;
	
	UDirectionalLightComponent* DLC =
		Cast<UDirectionalLightComponent>(KeyLightActor->GetLightComponent());
	
	if (!DLC) return;

	// From source to scene (-)
	const FVector LightDirWS = (-KeyLightActor->GetActorForwardVector()).GetSafeNormal();
	
	
	const FLinearColor LightColor = DLC->GetLightColor();
	const float Intensity = DLC->Intensity; 
	const FLinearColor Scaled = LightColor * (Intensity * IntensityScale);

	// Write
	MPCInstance->SetVectorParameterValue(ParamKeyLightDir,
		FLinearColor(LightDirWS.X, LightDirWS.Y, LightDirWS.Z, 1.0f));

	MPCInstance->SetVectorParameterValue(ParamKeyLightColor, Scaled);
}