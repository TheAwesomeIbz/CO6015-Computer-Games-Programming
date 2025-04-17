// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraControllerActor.generated.h"

/// <summary>
/// Camera controller actor responsible for adding camera shake and earthquake effects onto the main camera actor
/// </summary>
UCLASS()
class CO6015_API ACameraControllerActor : public AActor
{
	GENERATED_BODY()

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	ACameraActor* c_GlobalCameraActor;
	float f_shakeAmplitude;
	float f_shakeDuration;

	FVector v_originalPosition;
	FRotator r_originalRotation;
	int f_earthquakeIntensity;
	float f_earthquakeDuration;

	/// <summary>
	/// Update method called in the tick method to update the camera shake effect
	/// </summary>
	/// <param name="_deltaTime"></param>
	void CameraShakeTick(float _deltaTime);

	/// <summary>
	/// Update method called in the tick method to update the earthquake effect
	/// </summary>
	/// <param name="_deltaTime"></param>
	void EarthquakeTick(float _deltaTime);

public:	
	// Sets default values for this actor's properties
	ACameraControllerActor();

	/// <summary>
	/// Set the camera shaking properties
	/// </summary>
	/// <param name="_amplitude"></param>
	/// <param name="_duration"></param>
	void SetCameraShake(float _amplitude, float _duration = 0.25f);

	/// <summary>
	/// Sets the earthquake effect properties
	/// </summary>
	/// <param name="_intensity"></param>
	/// <param name="_duration"></param>
	void EarthquakeEffect(float _intensity = 10.0f, float _duration = 0.25f);

	virtual void Tick(float DeltaTime) override;

};
