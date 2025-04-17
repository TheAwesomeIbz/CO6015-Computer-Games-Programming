// Fill out your copyright notice in the Description page of Project Settings.


#include "Pill.h"

// Sets default values
APill::APill()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	visualComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	visualComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Engine/BasicShapes/Sphere.sphere"));
	static ConstructorHelpers::FObjectFinder<UMaterial> mat(TEXT("/Game/Materials/M_Pink.M_Pink"));
	if (meshAsset.Succeeded())
	{
		visualComponent->SetStaticMesh(meshAsset.Object);
		visualComponent->SetMaterial(0, mat.Object);
		visualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		visualComponent->SetWorldScale3D(FVector(PILLSCALE));
	}
	powerpill = false;

}

// Called when the game starts or when spawned
void APill::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void APill::SetPowerpill(bool power)
{
	powerpill = power;
	if (powerpill)
	{
		visualComponent->SetWorldScale3D(FVector(PILLSCALE*2.0f));
	}
}
