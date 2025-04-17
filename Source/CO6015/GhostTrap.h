#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostTrap.generated.h"

class AMain;

/// <summary>
/// Actor class representing collectable objects that are randomly placed in the game world
/// </summary>
UCLASS()
class CO6015_API AGhostTrap : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	UStaticMeshComponent* visualComponent;
	AMain* GameMode;
	virtual void BeginPlay() override;

	FVector vOriginalPosition;
	float f_OscillationValue;
	int i_PeriodMultiplier = 2;
	int i_AmplitudeMultiplier = 20;

	/// <summary>
	/// Uupdate method called in tick that makes a floating animation for the trap object
	/// </summary>
	/// <param name="_deltaTime"></param>
	void FloatTick(float _deltaTime);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	AGhostTrap();
};
