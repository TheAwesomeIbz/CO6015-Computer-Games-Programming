// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Pacman.h"
#include "Engine.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UIDisplay.generated.h"


/// <summary>
/// Actor class used to display the UI elements of the game. These elements exist as 3D text objects in the game world.
/// </summary>
UCLASS()
class CO6015_API AUIDisplay : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_titleText;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_descriptionText;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_previousHighScores;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_accessibilityTitleText;
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))

	class UTextRenderComponent* T_gameToggleText;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_scoreText;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_highScoreHeading;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_highScoreValue;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_currentPowerup;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_liveCounter;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_currentLevel;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_pacManPoints;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_accessibilityModeDisplay;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// <summary>
	/// Initiallises the text render component by setting up attachment to the root component, setting the relative rotation, scale, and alignment.
	/// </summary>
	/// <param name="renderComponent"></param>
	/// <param name="fromText"></param>
	void CreateSubObject(UTextRenderComponent* renderComponent, FText fromText);

public:	
	// Sets default values for this actor's properties
	AUIDisplay();
	virtual void Tick(float DeltaTime) override;

	/// <summary>
	/// Display the Title UI elements by setting the text to the default values.
	/// </summary>
	void ShowTitleUI();

	/// <summary>
	/// Hide the Title UI elements by setting the text to an empty string.
	/// </summary>
	void HideTitleUI();

	/// <summary>
	/// Displays and set the default values for the Game UI.
	/// </summary>
	void ShowGameUI();

	/// <summary>
	/// Hides the Game UI by setting all text elements to empty strings.
	/// </summary>
	void HideGameUI();

	/// <summary>
	/// Updates the text to display the current points of the pacman on the screen, if pacman exists.
	/// </summary>
	void SetPacmanPoints(APacman* pacman, int points);

	/// <summary>
	/// Updates the text to display the current colour blindness mode on the screen.
	/// </summary>
	void UpdateAccessibilityMode(bool mode, int colourBlindnessMode);


	void UpdateCurrentLevel(int level);
	void UpdateCurrentScore(int scoreValue);
	void UpdateHighScore(int scoreValue);

	/// <summary>
	/// Update the UI color and text of the random powerups that pacman can receive throughout the game.
	/// </summary>
	void UpdateCurrentPowerup(APacman* pacman);

	/// <summary>
	/// Updates the lives display for pacman. this will also update the render color of the text, depending on how many lives he has left.
	/// </summary>
	void UpdateLives(int playerLives);

	/// <summary>
	/// Updates the UI to display further instructions, once the player loses all their lives.
	/// </summary>
	void OnPlayerLoseUI(int score, int highScore);
};
