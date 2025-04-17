// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "CameraControllerActor.h"
#include "UIDisplay.h"
#include "GameFramework/GameModeBase.h"
#include "Structures.h"
#include "Main.generated.h"

class AAccessibilityUIActor;
class AUIDisplay;
class APacman;
class AGhost;
class AGhostTrap;

enum
{
	N_WALL = 0, N_PILL, N_EMPTY, N_POWERPILL, N_SLOW, N_PACMAN, N_GHOST, N_BLINKY, N_PINKY, N_INKY, N_CLYDE
};

/// <summary>
/// Singleton Game Mode object that acts as the main controller for the game. This keeps track of all ghosts, the player, the maze, and the game state.
/// </summary>
UCLASS()
class CO6015_API AMain : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	ACameraControllerActor* cameraController;

	// Maze grid codes - see enum above
	//
	// 0 = wall
	// 1 = standard node (with pill)
	// 2 = standard node (without pill)
	// 3 = standard node (with power pill)
	// 4 = slow node (to ghosts)
	// 5 = Pacman start node
	// 6 = ghost only node
	// 7 = Blinky start node
	// 8 = Pinky start node
	// 9 = Inky start node
	// 10 = Clyde start node

	int maze[GRIDY][GRIDX] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
								0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
								0, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 0,
								0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
								0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
								0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
								0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
								0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 6, 6, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 6, 6, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								4, 4, 4, 4, 4, 4, 1, 2, 2, 2, 0, 0, 9, 8, 7,10, 0, 0, 2, 2, 2, 1, 4, 4, 4, 4, 4, 4,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0,
								0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
								0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
								0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
								0, 3, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 3, 0,
								0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0,
								0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0,
								0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0,
								0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
								0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
								0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
								0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	int mazeNodes[GRIDY][GRIDX];
	TArray<navNode> navMesh;
	
	APacman* pacman;
	AGhost* ghostBlinky;
	AGhost* ghostPinky;
	AGhost* ghostInky;
	AGhost* ghostClyde;
	AAccessibilityUIActor* accessibilityUI;
	float offsetX;
	float offsetY;
	int i_nodeID;
	int i_nodeScatterBlinky;		// Top right node
	int i_nodeScatterPinky;		// Top left node
	int i_nodeScatterInky;		// Bottom right node
	int i_nodeScatterClyde;		// Bottom left node
	int i_nodeBlinky;		// Blinky start node
	int i_nodePinky;		// Pinky start node
	int i_nodeInky;		// Inky start node
	int i_nodeClyde;		// Clyde start node
	int i_nodePacman;		// Pacman start node
	int i_nodeLeft;		// Teleport Left node
	int i_nodeRight;		// Teleport Right node
	int i_pillsRemaining;
	int i_maximumPills;
	int i_score;
	int i_colourBlindnessMode;
	int i_playerLivesRemaining;
	int i_currentLevel;
	int i_amountOfGhostTrapsInLevel;
	bool b_scatter;
	bool b_inkyIsReleased;
	bool b_clydeIsReleased;
	bool b_accessibilityMode;
	bool b_gameStarted;
	bool b_playerLost;

	/// <summary>
	/// Spawn pills, and record them in the navMesh
	/// </summary>
	void ResetPills();

	/// <summary>
	/// Update the state of the ghosts, depending on the number of pills eaten
	/// </summary>
	void UpdateGhostStates();

public:

	AMain();
	virtual void Tick(float DeltaTime) override;

	AUIDisplay* UIDisplay;

	/// <summary>
	/// Spawns particles at a specified actor location with a specified amount and type of particle
	/// </summary>
	/// <param name="actorPointer">: Pointer to actor</param>
	/// <param name="amount">: Amount of particles to be spawned</param>
	/// <param name="particleType">: Type (colour) of particle to be spawned</param>
	void SpawnParticles(AActor* actorPointer, int amount = 5, int particleType = NORMAL_PARTICLE);

	/// <summary>
	/// Add points to the player's score
	/// </summary>
	/// <param name="points">: points to be added</param>
	void AddPoints(int points);

	/// <summary>
	/// Set the camera shake to a specified intensity and duration
	/// </summary>
	/// <param name="intensity">: amplitude of camera shake</param>
	/// <param name="duration">: duration for how long the shake should occur for</param>
	void ShakeCamera(float intensity, float duration = 0.25f) { if (!b_accessibilityMode) cameraController->SetCameraShake(intensity, duration); }

	/// <summary>
	/// Set the earthquake effect to a specified intensity and duration
	/// </summary>
	/// <param name="earthQuakeAmplitude">: amplitude of earthquake</param>
	/// <param name="earthQuakeDuration">: duration for how long the shake should occur for</param>
	void EarthquakeEffect(float earthQuakeAmplitude, float earthQuakeDuration = 0.25f);

	/// <summary>
	/// Increment the colour blindness mode through various modes
	/// </summary>
	void IncrementColourBlindnessMode();

	/// <summary>
	/// Reset the colour blindness mode to the default vision
	/// </summary>
	void ResetColourBlindnessMode();

	/// <returns>Whether half the pills are consumed</returns>
	bool HalfOfPillsConsumed() { return i_pillsRemaining <= i_maximumPills / 2; }

	/// <returns>Whether the game has started or not</returns>
	bool GameStarted() { return b_gameStarted; }

	/// <summary>
	/// Starts the current game
	/// </summary>
	void StartGame() { b_gameStarted = true; }

	/// <summary>
	/// Toggles the accessibility mode
	/// </summary>
	void ToggleAccessibility();

	/// <returns>Whether the accessibility mode is enabled or not</returns>
	bool IsAccessibilityModeEnabled() { return b_accessibilityMode; }

	/// <summary>
	/// Adds a ghost trap to the game if the current amount of ghost traps in the game is below the maximum amount of ghost traps
	/// </summary>
	void AddGhostTrap();

	/// <summary>
	/// Removes a ghost trap within the game
	/// </summary>
	void RemoveGhostTrap();

	/// <returns>Whether the amount of ghost traps is below the maximum amount of traps permitted</returns>
	bool CanAddGhostTrap() { return i_amountOfGhostTrapsInLevel < MAXIMUM_GHOST_TRAPS; }

	/// <summary>
	/// Called when the player collected all the pills in the game
	/// </summary>
	void OnPlayerWin();

	/// <returns>Whether all the pills are collected within the game</returns>
	bool AllPillsCollected() { return i_pillsRemaining == 0; }

	/// <summary>
	/// Returns a ghost by a specified Ghost index
	/// </summary>
	/// <param name="ghostValue"></param>
	/// <returns></returns>
	AGhost* GetGhost(int ghostValue);

	/// <summary>
	/// Returns the scatter node by a specified Ghost index
	/// </summary>
	/// <param name="ghostValue"></param>
	/// <returns></returns>
	int GetScatterNode(int ghostValue);

	/// <summary>
	/// Returns the starting node by a specified Ghost index
	/// </summary>
	int GetStartingNode(int ghostValue);

	/// <summary>
	/// Set the state of ghost entities in the game
	/// </summary>
	/// <param name="pausedState"></param>
	void SetGhostPaused(bool pausedState);

	/// <summary>
	/// Sets the state that the game should be paused at
	/// </summary>
	/// <param name="pausedState"></param>
	void SetGamePaused(bool pausedState);

	/// <returns>The adjacent node in the direction from a parameter node</returns>
	int AdjacentNode(int from, int direction);
	
	/// <returns>Whether this </returns>
	bool IsJunctionNode(int node) { return navMesh[node].isJunction; }

	/// <returns>Whether this node is a node that only ghosts can be in</returns>
	bool IsGhostOnlyNode(int node) { return navMesh[node].ghostOnly; }
	
	/// <returns>Whether this node is a node slow for ghosts</returns>
	bool IsSlowNode(int node) { return navMesh[node].slowNode; }
	
	/// <returns>The vector position of the current position</returns>
	FVector GetNodePos(int node);

	/// <returns>The nearest node to the vector position provided</returns>
	int FindNearestNode(FVector pos, bool junction = false);
	
	/// <returns>The node which pacman is on</returns>
	int GetPacmanNode();

	/// <returns>Get the direction from the current node to the adjacent node</returns>
	int GetNodeDirection(int from, int to);

	/// <summary>
	/// Toggles the scatter mode of the ghosts
	/// </summary>
	void ToggleScatterMode();

	int GetLeftNode() { return i_nodeLeft; }
	int GetRightNode() { return i_nodeRight; }

	/// <summary>
	/// Reset the maze to its original state and all the ghosts and pacman
	/// </summary>
	void ResetMaze();

	/// <returns>Get the pacman actor</returns>
	APacman* GetPacman();

	/// <summary>
	/// Eat a pill at a node if a pill exists within the node
	/// </summary>
	/// <param name="node"></param>
	void EatPill(int node);
	
	/// <summary>
	/// Chugani (2024). Euclidean distance formula, calculated by the square root of the sum of the squares of the differences between the x and y coordinates of two points.
	/// </summary>
	/// <returns>The distance between two nodes</returns>
	int EuclideanDistance(int from, int to);

	/// <returns>The current player score</returns>
	int GetPlayerScore() { return i_score; }

	/// <returns>The lives that the player currently has.</returns>
	int GetPlayerLives() { return i_playerLivesRemaining; }

	/// <summary>
	/// Decrements the player's lives by 1
	/// </summary>
	void ReducePlayerLives() { i_playerLivesRemaining--; }

	/// <returns>A random point within the maze that can be travelled to by the player</returns>
	FVector GenerateRandomPoint();

	/// <summary>
	/// This is called when the player has lost all their lives, and has to reset the game and all the pills.
	/// </summary>
	void OnPlayerLose();

	/// <summary>
	/// Called whenever a Power Pill is consumed within the game. This must frighten all the ghosts within the game.
	/// </summary>
	void OnPowerPillConsumed();

	/// <returns>Whether the player has lost the game or not.</returns>
	bool PlayerLost() { return b_playerLost; }
};
