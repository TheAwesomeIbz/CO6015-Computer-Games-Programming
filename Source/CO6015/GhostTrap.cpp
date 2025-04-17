#define MATH_PI (3.14159265358979323846f)

#include "GhostTrap.h"
#include "Main.h"
#include "Pacman.h"
#include "ParticleActor.h"

// Sets default values
AGhostTrap::AGhostTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	visualComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	visualComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Engine/BasicShapes/Cube.cube"));
	static ConstructorHelpers::FObjectFinder<UMaterial> ghostTrapMaterial(TEXT("/Game/Materials/M_GhostTrap.M_GhostTrap"));

	if (meshAsset.Succeeded())
	{
		visualComponent->SetStaticMesh(meshAsset.Object);
		visualComponent->SetMaterial(0, ghostTrapMaterial.Object);
		visualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		visualComponent->SetWorldScale3D(FVector(0.5f));
	}
}

// Called when the game starts or when spawned
void AGhostTrap::BeginPlay()
{
	Super::BeginPlay();

	GameMode = (AMain*)(GetWorld()->GetAuthGameMode());
	f_OscillationValue = 0;
	vOriginalPosition = GetActorLocation();
}

// Called every frame
void AGhostTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Do not continue if pacman does not exist
	if (!GameMode->GetPacman()) { return; }

	//Check if pacman is close enough to the trap to trigger the effect
	FVector separation = this->GetActorLocation() - GameMode->GetPacman()->GetActorLocation();
	bool collidedWithPacman = separation.SizeSquared2D() < (COLLISION * COLLISION);

	if (collidedWithPacman)
	{
		GWorld->SpawnActor<AParticleActor>(AParticleActor::StaticClass(), GetActorLocation(), GetActorRotation());

		//generate a random effect integer between 0 and 6
		int randomEffect = rand() % 7;
		while (randomEffect == GameMode->GetPacman()->GetRandomEffect()){
			randomEffect = rand() % 6;
		}

		GameMode->AddPoints(600);
		GameMode->GetPacman()->SetRandomEffect(randomEffect);
		GameMode->RemoveGhostTrap();
		GameMode->ShakeCamera(10);
		Destroy();
	}

	FloatTick(DeltaTime);
}

void AGhostTrap::FloatTick(float _deltaTime)
{
	f_OscillationValue += _deltaTime;

	SetActorLocation(FVector(vOriginalPosition.X, vOriginalPosition.Y + 
		FMath::Sin(f_OscillationValue * i_PeriodMultiplier) * i_AmplitudeMultiplier, 
		vOriginalPosition.Z));

	if (f_OscillationValue > MATH_PI * 2)
		f_OscillationValue = 0;
}

