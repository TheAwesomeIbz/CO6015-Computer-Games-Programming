// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structures.h"
#include "Pill.generated.h"

UCLASS()
class CO6015_API APill : public AActor
{
	GENERATED_BODY()
	
private:
	UStaticMeshComponent* visualComponent;
	bool powerpill;

public:	
	// Sets default values for this actor's properties
	APill();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetPowerpill(bool power);
	bool IsPowerpill() { return powerpill; }
};
