// Fill out your copyright notice in the Description page of Project Settings.

#include "UIDisplay.h"
#include "Ghost.h"
#include "Main.h"
#include "SavingSystem.h"

#define PACMAN_POINTS_OFFSET (50.0f)


// Sets default values
AUIDisplay::AUIDisplay()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	T_titleText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("title"));
	CreateSubObject(T_titleText, FText::FromString("CO6015 Computer Games and Programming"));

	T_descriptionText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("description"));
	CreateSubObject(T_descriptionText, FText::FromString("Pacman Randomizer Nuzlocke"));

	T_accessibilityTitleText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("accessibility"));
	CreateSubObject(T_accessibilityTitleText, FText::FromString("PRESS [CTRL] TO TOGGLE THE ACCESSIBILITY MODE"));

	T_gameToggleText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("gametoggle"));
	CreateSubObject(T_gameToggleText, FText::FromString("PRESS [ENTER] TO START PLAYING"));

	T_scoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("score"));
	CreateSubObject(T_scoreText, FText::FromString("00"));

	T_highScoreHeading = CreateDefaultSubobject<UTextRenderComponent>(TEXT("highScoreHeading"));
	CreateSubObject(T_highScoreHeading, FText::FromString("00"));

	T_highScoreValue = CreateDefaultSubobject<UTextRenderComponent>(TEXT("highScoreValue"));
	CreateSubObject(T_highScoreValue, FText::FromString("High Score"));

	T_currentPowerup = CreateDefaultSubobject<UTextRenderComponent>(TEXT("powerup"));
	CreateSubObject(T_highScoreValue, FText::FromString("POWERUP : NONE"));

	T_currentLevel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("level"));
	CreateSubObject(T_currentLevel, FText::FromString("LEVEL 1"));

	T_liveCounter = CreateDefaultSubobject<UTextRenderComponent>(TEXT("lives"));
	CreateSubObject(T_liveCounter, FText::FromString("LIVES : 3"));

	T_pacManPoints = CreateDefaultSubobject<UTextRenderComponent>(TEXT("pacmanPoints"));
	CreateSubObject(T_pacManPoints, FText::FromString("200"));

	T_accessibilityModeDisplay = CreateDefaultSubobject<UTextRenderComponent>(TEXT("currentAccessibilityMode"));
	CreateSubObject(T_accessibilityModeDisplay, FText::FromString("[ACCESSIBILITY MODE DISABLED]"));

	ShowTitleUI();
	HideGameUI();
}
// Called when the game starts or when spawned
void AUIDisplay::BeginPlay()
{
	Super::BeginPlay();
	ShowTitleUI();
	HideGameUI();

}

void AUIDisplay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUIDisplay::CreateSubObject(UTextRenderComponent* renderComponent, FText fromText)
{
	renderComponent->SetupAttachment(RootComponent);

	//sets the rotation and scale to constant values that are displayed tot ehe screen
	//these values are overriden by the inspector positions and rotations
	//but are necessary for initialisation
	renderComponent->SetRelativeRotation(FRotator(90.0f, 90.0f, 0.0f));
	renderComponent->SetRelativeScale3D(FVector(2.0f, 2.0f, 2.0f));

	renderComponent->SetText(fromText);
}

void AUIDisplay::SetPacmanPoints(APacman* pacman, int points)
{
	T_pacManPoints->SetText(pacman ?
		FText::FromString(FString::FromInt(points)) : FText::FromString(FString("")));
	if (!pacman)
		return;

	T_pacManPoints->SetRelativeLocation(pacman->GetActorLocation() + FVector(0.0f, 0.0f, PACMAN_POINTS_OFFSET));
}

void AUIDisplay::ShowTitleUI()
{
	T_titleText->SetText(FText::FromString("CO6015 Computer Games and Programming"));
	T_descriptionText->SetText(FText::FromString("Pacman Randomizer Nuzlocke"));
	T_accessibilityTitleText->SetText(FText::FromString("PRESS [CTRL] TO TOGGLE THE ACCESSIBILITY MODE"));
	T_gameToggleText->SetText(FText::FromString("PRESS [ENTER] TO START PLAYING"));
	SetPacmanPoints(NULL, 0);
}

void AUIDisplay::HideTitleUI()
{
	T_titleText->SetText(FText::FromString(""));
	T_descriptionText->SetText(FText::FromString(""));
	T_accessibilityTitleText->SetText(FText::FromString(""));
	T_gameToggleText->SetText(FText::FromString(""));
	T_accessibilityTitleText->SetText(FText::FromString(""));
}

void AUIDisplay::ShowGameUI()
{
	T_scoreText->SetText(FText::FromString("00"));
	T_highScoreHeading->SetText(FText::FromString("HIGH SCORE"));

	SavingSystem saveObject;
	UpdateHighScore(saveObject.LoadHighScore());
	T_currentPowerup->SetText(FText::FromString("POWERUP : NONE"));
	T_liveCounter->SetText(FText::FromString("LIVES : 3"));
	T_currentLevel->SetText(FText::FromString("LEVEL 1"));
	T_accessibilityModeDisplay->SetText(FText::FromString("ACCESSIBILITY MODE DISABLED - [DEFAULT]"));


}

void AUIDisplay::UpdateAccessibilityMode(bool mode, int colourBlindnessMode)
{
	FString colourBlindness;
	switch (colourBlindnessMode)
	{
	case DEUTERANOPIA:
		colourBlindness = FString("[DEUTERANOPIA]");
		break;
	case PROTANOPIA:
		colourBlindness = FString("[PROTANOPIA]");
		break;
	case TRITANOPIA:
		colourBlindness = FString("[TRITANOPIA]");
		break;
	case MONOCHROMACY:
		colourBlindness = FString("[MONOCHROMACY]");
		break;
	case DEFAULT_VISION:
	default:
		colourBlindness = FString("[DEFAULT]");
		break;
	}

	FString accessibilityText = mode ? FString("ACCESSIBILITY MODE ENABLED - ") : FString("ACCESSIBILITY MODE DISABLED - ");
	T_accessibilityModeDisplay->SetText(FText::FromString(accessibilityText + colourBlindness));
	T_accessibilityModeDisplay->TextRenderColor = mode ? FColor::Green : FColor::Red;
}

void AUIDisplay::HideGameUI()
{
	T_scoreText->SetText(FText::FromString(""));
	T_highScoreHeading->SetText(FText::FromString(""));
	T_highScoreValue->SetText(FText::FromString(""));
	T_currentPowerup->SetText(FText::FromString(""));
	T_liveCounter->SetText(FText::FromString(""));
	T_currentLevel->SetText(FText::FromString(""));
	T_accessibilityModeDisplay->SetText(FText::FromString(""));
}

void AUIDisplay::UpdateCurrentScore(int scoreValue)
{
	T_scoreText->SetText(FText::FromString(FString::FromInt(scoreValue)));
}

void AUIDisplay::UpdateHighScore(int scoreValue)
{
	T_highScoreValue->SetText(FText::FromString(FString::FromInt(scoreValue)));
}

void  AUIDisplay::UpdateCurrentLevel(int level)
{
	FString currentLevel = FString("LEVEL ") + FString::FromInt(level);
	T_currentLevel->SetText(FText::FromString(currentLevel));
}

void AUIDisplay::UpdateCurrentPowerup(APacman* pacman)
{
	if (pacman->IsPaused())
	{
		T_currentPowerup->SetText(FText::FromString(""));
		return;
	}

	FString currentPowerup;

	if (pacman->InvertedControls())
	{
		currentPowerup = FString("INVERTED CONTROLS");
	}
	else switch (pacman->GetRandomEffect())
	{
	case EFFECT_HALFPOINTS:
		currentPowerup = FString("HALVED POINTS");
		break;

	case EFFECT_SPEEDINCREASE:
		currentPowerup = FString("+ SPEED");
		break;
	case EFFECT_SPEEDDECREASE:
		currentPowerup = FString("- SPEED");
		break;
	case EFFECT_FREEZEGHOST:
		currentPowerup = FString("FROZEN GHOSTS");
		break;
	case EFFECT_EARTHQUAKE:
		currentPowerup = FString("EARTHQUAKE");
		break;
	default:
		currentPowerup = FString("NONE");
		break;
	}

	//visually display the half points effect displayed to the screen
	if (pacman->GetRandomEffect() == EFFECT_HALFPOINTS)
	{
		T_scoreText->TextRenderColor = FColor::Red;
	}
	else
	{
		T_scoreText->TextRenderColor = FColor::White;
	}

	///get the duration left for all powerups except for freeze ghosts.
	//EFFECT_FREEZEGHOST runs on internal timers within the ghosts, even if pacman runs into more ghost traps
	float durationLeft = pacman->DurationLeft();
	FString durationString = pacman->GetRandomEffect() != -1 && pacman->GetRandomEffect() != EFFECT_FREEZEGHOST ? FString(" (") + FString::FromInt(durationLeft) + FString("s)") : FString("");

	FString powerupDisplay = FString("POWERUP : ") + currentPowerup + durationString;

	T_currentPowerup->SetText(FText::FromString(powerupDisplay));
}

void AUIDisplay::UpdateLives(int playerLives)
{
	FString livesDisplay = FString("LIVES : ") + FString::FromInt(playerLives);

	switch (playerLives)
	{
	case 3:
	{
		T_liveCounter->TextRenderColor = FColor::White;
		break;
	}
	case 2:
	{
		T_liveCounter->TextRenderColor = FColor::Yellow;
		break;
	}
	case 1:
	default:
	{
		T_liveCounter->TextRenderColor = FColor::Red;
		break;
	}
	}
	
	T_liveCounter->SetText(FText::FromString(livesDisplay));
}

void AUIDisplay::OnPlayerLoseUI(int score, int highScore)
{
	T_liveCounter->SetText(FText::FromString("LIVES : 0"));
	T_currentLevel->TextRenderColor = FColor::Red;
	T_currentLevel->SetText(FText::FromString("GAME OVER!"));

	//updates the text display to show whether the high score was beaten on this session or not
	FString scoreText;
	if (highScore >= score)
	{
		T_currentPowerup->TextRenderColor = FColor::Yellow;
		FString highScoreDifference = FString::FromInt(highScore - score);
		scoreText = FString("POINTS NEEDED FOR HIGH SCORE : ") + highScoreDifference + FString(" POINTS");
	}
	else
	{
		T_currentPowerup->TextRenderColor = FColor::Green;
		scoreText = FString("NEW HIGH SCORE REACHED AT ") + FString::FromInt(score);
	}
	T_currentPowerup->SetText(FText::FromString(scoreText));

	T_accessibilityModeDisplay->TextRenderColor = FColor::Green;
	T_accessibilityModeDisplay->SetText(FText::FromString("PRESS [ENTER] TO RETURN BACK TO THE MAIN MENU."));
}




