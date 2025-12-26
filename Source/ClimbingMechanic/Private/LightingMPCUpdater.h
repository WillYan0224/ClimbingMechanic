#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightingMPCUpdater.generated.h"

class ADirectionalLight;
class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;

UCLASS()
class ALightingMPCUpdater : public AActor
{
	GENERATED_BODY()

public:
	ALightingMPCUpdater();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	// Pin the main Directional Light
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MPC")
	TObjectPtr<ADirectionalLight> KeyLightActor = nullptr;

	// MPC Asset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MPC")
	TObjectPtr<UMaterialParameterCollection> LightingMPC = nullptr;

	// MPC Param (Must match MPC in mat)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MPC")
	FName ParamKeyLightDir = TEXT("KeyLightDir_WS");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MPC")
	FName ParamKeyLightColor = TEXT("KeyLightColor");

	// Directional Light
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MPC")
	float IntensityScale = 0.001f;

	// cutscene Swap light: true : otherwise false
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MPC")
	bool bTickUpdate = true;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialParameterCollectionInstance> MPCInstance = nullptr;

	void UpdateMPCOnce();
};
