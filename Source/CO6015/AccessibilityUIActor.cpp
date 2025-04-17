#include "AccessibilityUIActor.h"
#include "Main.h"
#include "Engine.h"
#include "GhostTrap.h"
#include "Structures.h"
#include "Ghost.h"

#define TITLE_OFFSET (-75)
#define SUBTITLE_OFFSET (-20)

#define OUT_OF_BOUNDS (-200)
#define INSIDE_BOUNDS (40)


/// <summary>
/// Initialise all text render components, including the text and subtitle they display.
/// </summary>
AAccessibilityUIActor::AAccessibilityUIActor()
{
	PrimaryActorTick.bCanEverTick = true;

	T_pacmanDisplay = CreateDefaultSubobject<UTextRenderComponent>(TEXT("pacMan"));
	CreateSubObject(T_pacmanDisplay, FText::FromString("PACMAN"));

	T_ghostInky = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Inky"));
	CreateSubObject(T_ghostInky, FText::FromString("INKY"));
	T_ghostInkySubtitle = CreateDefaultSubobject<UTextRenderComponent>(TEXT("inkySubtitle"));
	CreateSubObject(T_ghostInkySubtitle, FText::FromString(""));

	T_ghostBlinky = CreateDefaultSubobject<UTextRenderComponent>(TEXT("blinky"));
	CreateSubObject(T_ghostBlinky, FText::FromString("BLINKY"));
	T_ghostBlinkySubtitle = CreateDefaultSubobject<UTextRenderComponent>(TEXT("blinkySubtitle"));
	CreateSubObject(T_ghostBlinkySubtitle, FText::FromString(""));

	T_ghostPinky = CreateDefaultSubobject<UTextRenderComponent>(TEXT("pinky"));
	CreateSubObject(T_ghostPinky, FText::FromString("PINKY"));
	T_ghostPinkySubtitle = CreateDefaultSubobject<UTextRenderComponent>(TEXT("pinkySubtitle"));
	CreateSubObject(T_ghostPinkySubtitle, FText::FromString(""));

	T_ghostClyde = CreateDefaultSubobject<UTextRenderComponent>(TEXT("clyde"));
	CreateSubObject(T_ghostClyde, FText::FromString("CLYDE"));
	T_ghostClydeSubtitle = CreateDefaultSubobject<UTextRenderComponent>(TEXT("clydeSubtitle"));
	CreateSubObject(T_ghostClydeSubtitle, FText::FromString(""));
}

void AAccessibilityUIActor::BeginPlay()
{
	Super::BeginPlay();

	InitialiseColours();
	GameMode = (AMain*)(GetWorld()->GetAuthGameMode());	

	// Find the camera component within the scene
	for (TObjectIterator<ACameraActor> act; act; ++act) {

		if (act->GetWorld() == this->GetWorld()) {
			cameraComponent = act->GetCameraComponent();
			break;
		}
	}
}

void AAccessibilityUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GameMode) return;

	T_pacmanDisplay->SetRelativeLocation(GameMode->GetPacman()->GetActorLocation() + FVector(0, TITLE_OFFSET, GameMode->IsAccessibilityModeEnabled() ? INSIDE_BOUNDS : OUT_OF_BOUNDS));
	UpdateGhostAccessibilityUI();
}

void AAccessibilityUIActor::CreateSubObject(UTextRenderComponent* renderComponent, FText fromText)
{
	renderComponent->SetupAttachment(RootComponent);

	renderComponent->SetRelativeRotation(FRotator(90.0f, 90.0f, 0.0f));
	renderComponent->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	renderComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	renderComponent->SetText(fromText);
}

void AAccessibilityUIActor::InitialiseColours()
{
	FVector4 defaultVision = FVector4(1, 1, 1, 1);
	FVector4 deuteranopia = FVector4(1, 0, 1, 1);
	FVector4 protanopia = FVector4(0, 1, 1, 1);
	FVector4 tritanopia = FVector4(1, 1, 0, 1);
	FVector4 monochromacy = FVector4(0, 0, 0, 1);

	SaturationColours = new FVector4[5]{ defaultVision, deuteranopia, protanopia, tritanopia, monochromacy };
}

void AAccessibilityUIActor::SetColourBlindnessMode(int mode)
{
	cameraComponent->PostProcessSettings.ColorSaturation = SaturationColours[mode];
}

void AAccessibilityUIActor::SetGhostTitle(UTextRenderComponent* renderComponent, int _ghostValue)
{
	//Sets the text in front or behind the panel, depending on whether accessibility mode is enabled
	renderComponent->SetRelativeLocation(GameMode->GetGhost(_ghostValue)->GetActorLocation() + FVector(0, TITLE_OFFSET, GameMode->IsAccessibilityModeEnabled() ? INSIDE_BOUNDS : OUT_OF_BOUNDS));
}

void AAccessibilityUIActor::SetGhostSubtitle(UTextRenderComponent* subtitle, int _ghostValue)
{
	FString subtitleText;
	AGhost* ghost = GameMode->GetGhost(_ghostValue);
	if (ghost->IsEaten())
	{
		subtitleText = FString("EATEN");
	}
	else if (ghost->IsFrozen())
	{
		subtitleText = FString("FROZEN");
	}
	else if (ghost->IsInFrightenedMode())
	{
		subtitleText = FString("FRIGHTENED");
	}
	else if (ghost->IsPaused())
	{
		subtitleText = FString("IDLE");
	}
	else if (ghost->IsScattering())
	{
		subtitleText = FString("SCATTERING");
	}
	else
	{
		subtitleText = FString("ACTIVE");
	}

	subtitle->SetText(FText::FromString(subtitleText));

	//Sets the text in front or behind the panel, depending on whether accessibility mode is enabled
	subtitle->SetRelativeLocation(ghost->GetActorLocation() + FVector(0, SUBTITLE_OFFSET, GameMode->IsAccessibilityModeEnabled() ? INSIDE_BOUNDS : OUT_OF_BOUNDS));
}

void  AAccessibilityUIActor::UpdateGhostAccessibilityUI()
{
	SetGhostTitle(T_ghostInky, INKY);
	SetGhostTitle(T_ghostBlinky, BLINKY);
	SetGhostTitle(T_ghostClyde, CLYDE);
	SetGhostTitle(T_ghostPinky, PINKY);

	SetGhostSubtitle(T_ghostInkySubtitle, INKY);
	SetGhostSubtitle(T_ghostBlinkySubtitle, BLINKY);
	SetGhostSubtitle(T_ghostClydeSubtitle, CLYDE);
	SetGhostSubtitle(T_ghostPinkySubtitle, PINKY);
}


