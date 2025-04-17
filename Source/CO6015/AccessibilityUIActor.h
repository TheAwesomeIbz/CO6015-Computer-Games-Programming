// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "CoreMinimal.h"
#include "AccessibilityUIActor.generated.h"


class AMain;
class AGhost;

/// <summary>
/// Accessibility Actor class that handles the accessibility UI elements for the game. This includes the text render components for the ghosts and pacman, as well as the camera's colour saturation.
/// </summary>
UCLASS()
class CO6015_API AAccessibilityUIActor : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_pacmanDisplay;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_pacmanSubtitleDisplay;;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_ghostInky;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_ghostInkySubtitle;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_ghostBlinky;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_ghostBlinkySubtitle;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_ghostClyde;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_ghostClydeSubtitle;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_ghostPinky;

	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* T_ghostPinkySubtitle;

	UCameraComponent* cameraComponent;
	FVector4* SaturationColours;

	/// <summary>
	/// Initiallises the text render component by setting up attachment to the root component, setting the relative rotation, scale, and alignment.
	/// </summary>
	/// <param name="renderComponent"></param>
	/// <param name="fromText"></param>
	void CreateSubObject(UTextRenderComponent* renderComponent, FText fromText);

	/// <summary>
	/// Initialises the SaturationColours array with the different colour saturation values for each colour blindness mode.
	/// </summary>
	void InitialiseColours();

	/// <summary>
	/// Sets the title accessibility text for the ghost reference passed in.
	/// </summary>
	/// <param name="_renderComponent"></param>
	/// <param name="_ghostValue"></param>
	void SetGhostTitle(UTextRenderComponent* _renderComponent, int _ghostValue);

	/// <summary>
	/// Sets the subtitle accessibility text for the ghost reference passed in. This text depends on the current state of the ghost.
	/// </summary>
	/// <param name="_subtitle">: UTextRenderComponent object</param>
	/// <param name="_ghost">: Pointer to ghost object</param>
	void SetGhostSubtitle(UTextRenderComponent* _subtitle, int _ghostValue);

	/// <summary>
	/// Update method called in tick to update all ghost accessibility UI elements if the accessibility mode is enabled.
	/// </summary>
	void UpdateGhostAccessibilityUI();

	/// <summary>
	/// GameMode reference to the main game mode
	/// </summary>
	AMain* GameMode;

public:	
	
	/// <summary>
	/// Sets the camera's colour saturation, depending on the colour blindness mode
	/// </summary>
	/// <param name="_colourBlindnessMode">: value of colour blindness mode</param>
	void SetColourBlindnessMode(int _colourBlindnessMode);

	AAccessibilityUIActor();
	virtual void Tick(float DeltaTime) override;
};
