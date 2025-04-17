// Fill out your copyright notice in the Description page of Project Settings.


#include "Engine.h"
#include "CameraControllerActor.h"

ACameraControllerActor::ACameraControllerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	f_shakeAmplitude = 0.0f;
	f_shakeDuration = 0.0f;
	f_earthquakeIntensity = 0;
	f_earthquakeDuration = 0;
}

void ACameraControllerActor::BeginPlay()
{
	Super::BeginPlay();

	//Initialise camera actor within the scene
	for (TObjectIterator<ACameraActor> act; act; ++act) {
		if (act->GetWorld() == this->GetWorld()) {
			c_GlobalCameraActor = *act;
			break;
		}
	}
	v_originalPosition = c_GlobalCameraActor->GetActorLocation();
	r_originalRotation = c_GlobalCameraActor->GetActorRotation();
}

void ACameraControllerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (f_earthquakeDuration > 0.0f)
	{
		EarthquakeTick(DeltaTime);
		return;
	}
	CameraShakeTick(DeltaTime);
}

void ACameraControllerActor::SetCameraShake(float _amplitude, float _duration)
{
	f_shakeAmplitude = _amplitude;
	f_shakeDuration = _duration;
}

void ACameraControllerActor::EarthquakeEffect(float intesity, float _duration)
{
	f_earthquakeIntensity = intesity;
	f_earthquakeDuration = _duration;
}

void ACameraControllerActor::EarthquakeTick(float _deltaTime)
{
	f_earthquakeDuration -= _deltaTime;
	c_GlobalCameraActor->SetActorLocation(v_originalPosition + FVector(FMath::RandRange(-f_earthquakeIntensity, f_earthquakeIntensity), FMath::RandRange(-f_earthquakeIntensity, f_earthquakeIntensity), 0.0f));
}

void ACameraControllerActor::CameraShakeTick(float _deltaTime)
{
	if (f_shakeDuration > 0.0f)
	{
		f_shakeDuration -= _deltaTime;
		c_GlobalCameraActor->SetActorLocation(v_originalPosition + FVector(FMath::RandRange(-f_shakeAmplitude, f_shakeAmplitude), FMath::RandRange(-f_shakeAmplitude, f_shakeAmplitude), 0.0f));
	}
	else
	{
		c_GlobalCameraActor->SetActorLocation(v_originalPosition);
	}
}