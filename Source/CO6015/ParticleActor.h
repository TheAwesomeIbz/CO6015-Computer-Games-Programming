// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParticleActor.generated.h"

/// <summary>
/// Actor class that represents a single particle within the game. These particles will gradually decrease in size over their life and be automatically destroyed.
/// </summary>
UCLASS()
class CO6015_API AParticleActor : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UStaticMeshComponent* visualComponent;

	FVector velocity;
	float f_CurrentParticleLife;
	float f_OriginalParticleLife;

	UMaterial* particleMaterials[4];

public:	
	// Sets default values for this actor's properties
	AParticleActor();

	/// <summary>
	/// Set the material to the particle type enum
	/// </summary>
	/// <param name="particleType">: enum type stored within Structures.h</param>
	void InitialiseMaterial(int particleType = 0) { visualComponent->SetMaterial(0, particleMaterials[particleType]); }

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
