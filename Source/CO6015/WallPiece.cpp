// Fill out your copyright notice in the Description page of Project Settings.


#include "WallPiece.h"

// Sets default values
AWallPiece::AWallPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	visualComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	visualComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> straightMesh(TEXT("/Game/Meshes/SM_StraightBar.SM_StraightBar"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> curvedMesh(TEXT("/Game/Meshes/SM_CurvedBar.SM_CurvedBar"));
	straight = straightMesh.Object;
	curved = curvedMesh.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> materialAsset(TEXT("/Game/Materials/M_DarkBlue.M_DarkBlue"));
	if (straightMesh.Succeeded() && curvedMesh.Succeeded())
	{
		visualComponent->SetStaticMesh(straight);	// Default
		visualComponent->SetMaterial(0, materialAsset.Object);
		visualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		visualComponent->SetWorldScale3D(FVector(SCALE3DS));
	}
}


// Called when the game starts or when spawned
void AWallPiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWallPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWallPiece::SetCurved(bool isCurved)
{
	if (isCurved)
	{
		visualComponent->SetStaticMesh(curved);
	}
	else
	{
		visualComponent->SetStaticMesh(straight);
	}
}

void AWallPiece::SetThin()
{
	visualComponent->SetWorldScale3D(FVector(SCALE3DS*0.5f));
}

