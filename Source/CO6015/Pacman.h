// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "Structures.h"
#include "Pacman.generated.h"

class AMain;
class AGhost;

UCLASS()
class CO6015_API APacman : public APawn
{
	GENERATED_BODY()

private:
	UStaticMeshComponent* visualComponent;

	/// <returns>Whether there is a wall in the current direction</returns>
	bool WallInDirection(int direction);

	virtual void BeginPlay() override;

	void OnEnterButtonPressed();

	void OnControlButtonPressed();
	
	/// <summary>
	/// Update function called in tick that updates any random effect parameters
	/// </summary>
	/// <param name="_deltaTime"></param>
	void RandomEffectTick(float _deltaTime);

	/// <summary>
	/// Update function called in tick that moves pacman every frame
	/// </summary>
	/// <param name="_deltaTime"></param>
	void MovePacmanTick(float _deltaTime);

	/// <summary>
	/// Update function called in tick that checks for ghost collision
	/// </summary>
	/// <param name="_deltaTime"></param>
	void RegisterGhostCollisionTick(float _deltaTime);

	/// <summary>
	/// current random effect added onto the game mode
	/// </summary>
	int f_randomEffectMode;

	/// <summary>
	/// The duration in which the random effect will last
	/// </summary>
	float f_randomEffectTime;
	const float C_randomEffectDelay = 10.0f;

	float f_powerUpParticleCounter;
	const float f_powerUpParticleDelay = 0.125f;

	UMaterial* m_greenMaterial;
	UMaterial* m_yellowMaterial;

	AMain* GameMode;

	int i_currentNode;
	int i_previousNode;
	bool b_isMoving;
	FVector v_movingTo;
	int i_destinationNode;
	int i_movingDirection;
	
	/// <summary>
	/// Used to store the direction in which pacman travelled last for continuous movement.
	/// </summary>
	int i_cachedDirection;
	bool b_paused;

	TArray<AGhost*> ghosts;

public:	
	APacman();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool MoveNode(int direction);
	void MoveToNode(int toNode);

	/// <summary>
	/// Move pacman along the horizontal axis
	/// </summary>
	/// <param name="AxisValue"></param>
	void MoveX(float AxisValue);

	/// <summary>
	/// Move pacman along the vertical axis
	/// </summary>
	/// <param name="AxisValue"></param>
	void MoveY(float AxisValue);

	/// <summary>
	/// Reset pacman to the starting position and state
	/// </summary>
	void Reset();

	/// <returns>The current node Pacman is on</returns>
	int CurrentNode() { return i_currentNode; }

	/// <returns>The direction in which Pacman is facing</returns>
	int MovingDirection() { return i_movingDirection; }

	/// <summary>
	/// Set the random effect to the game mode
	/// </summary>
	void SetRandomEffect(int randomEffect);

	/// <returns>The current effect affected by Pacman</returns>
	int GetRandomEffect() { return f_randomEffectMode; }

	/// <returns>Whether Pacman has inverted controls or not</returns>
	bool InvertedControls() { return f_randomEffectMode <= EFFECT_INVERTEDCONTROLS && f_randomEffectMode > -1; }

	/// <returns>The amount of time left within the current random effect if it exists</returns>
	float DurationLeft() { return f_randomEffectTime; }

	/// <returns>Whether the player is paused or not</returns>
	bool IsPaused() { return b_paused; }

	/// <summary>
	/// Set the paused state of pacman
	/// </summary>
	void SetPaused(bool paused) { b_paused = paused; }

	/// <summary>
	/// Called whenever pacman collides with a ghost with the conditions of losing a life
	/// </summary>
	void OnLifeLost();

	/// <summary>
	/// Called whenever pacman collides with a ghost with the condition of successfully eating them
	/// </summary>
	/// <param name="ghost"></param>
	void OnGhostEaten(AGhost* ghost);

};
