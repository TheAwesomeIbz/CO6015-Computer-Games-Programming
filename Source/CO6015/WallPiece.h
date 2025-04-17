// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structures.h"
#include "WallPiece.generated.h"

UCLASS()
class CO6015_API AWallPiece : public AActor
{
	GENERATED_BODY()
	
private:
	UStaticMeshComponent* visualComponent;
	UStaticMesh* curved;
	UStaticMesh* straight;

public:	
	// Sets default values for this actor's properties
	AWallPiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetCurved(bool isCurved);
	void SetThin();

};
