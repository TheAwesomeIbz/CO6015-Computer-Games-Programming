// Fill out your copyright notice in the Description page of Project Settings.

#include "ParticleActor.h"
#include "Engine.h"
#include "Structures.h"
#define MAXIMUM_SPEED_SCALAR_VALUE (320)

// Sets default values
AParticleActor::AParticleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	visualComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	visualComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Engine/BasicShapes/Cube.cube"));

	static ConstructorHelpers::FObjectFinder<UMaterial> pMaterial(TEXT("/Game/Materials/M_Pink.M_Pink"));
	static ConstructorHelpers::FObjectFinder<UMaterial> yMaterial(TEXT("/Game/Materials/M_Yellow.M_Yellow"));
	static ConstructorHelpers::FObjectFinder<UMaterial> rMaterial(TEXT("/Game/Materials/M_Red.M_Red"));
	static ConstructorHelpers::FObjectFinder<UMaterial> gMaterial(TEXT("/Game/Materials/M_Green.M_Green"));

	particleMaterials[0] = pMaterial.Object;
	particleMaterials[1] = yMaterial.Object;
	particleMaterials[2] = rMaterial.Object;
	particleMaterials[3] = gMaterial.Object;

	//initialise mesh asset and material for object
	if (meshAsset.Succeeded())
	{
		visualComponent->SetStaticMesh(meshAsset.Object);
		visualComponent->SetMaterial(0, pMaterial.Object);
		visualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		visualComponent->SetWorldScale3D(FVector(0.5f));
	}
}

// Called when the game starts or when spawned
void AParticleActor::BeginPlay()
{
	Super::BeginPlay();

	//generate a random velocity vector from maximum speed scalar value
	velocity = FVector(
		(MAXIMUM_SPEED_SCALAR_VALUE / 2) - (rand() % MAXIMUM_SPEED_SCALAR_VALUE),
		(MAXIMUM_SPEED_SCALAR_VALUE / 2) - (rand() % MAXIMUM_SPEED_SCALAR_VALUE),
		0);

	//generate random particle life expectancy
	f_OriginalParticleLife = (rand() % 10) / 5.0f;
	f_CurrentParticleLife = f_OriginalParticleLife;
}

// Called every frame
void AParticleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//update the actor's location with respect to its velocity and each frame
	SetActorLocation(GetActorLocation() + (velocity * DeltaTime));

	float scale = (f_CurrentParticleLife / f_OriginalParticleLife) / 2.0f;
	f_CurrentParticleLife -= DeltaTime;

	//destroy automatically if the particle life hits zero
	if (f_CurrentParticleLife < 0){
		Destroy();
	}
	
	SetActorScale3D(FVector(scale));
}

