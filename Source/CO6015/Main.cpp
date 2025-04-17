// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "PacmanController.h"
#include "AccessibilityUIActor.h"
#include "Pacman.h"
#include "Ghost.h"
#include "Pill.h"
#include "WallPiece.h"
#include "GhostTrap.h"
#include "SavingSystem.h"
#include "ParticleActor.h"

AMain::AMain()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	// use our own player controller class
	PlayerControllerClass = APacmanController::StaticClass();
}

void AMain::BeginPlay()
{
	Super::BeginPlay();

	i_currentLevel = 1;
	b_playerLost = false;
	

	for (TObjectIterator<ACameraControllerActor> act; act; ++act) {

		if (act->GetWorld() == this->GetWorld()) 
		{
			cameraController = *act;
			break;
		}
	}

	for (TObjectIterator<AAccessibilityUIActor> act; act; ++act) {

		if (act->GetWorld() == this->GetWorld())
		{
			accessibilityUI = *act;
			break;
		}
	}

	for (TObjectIterator<AUIDisplay> act; act; ++act){

		if (act->GetWorld() == this->GetWorld()){
			UIDisplay = *act;
			break;
		}
	}

	AWallPiece* SpawnedWallPiece;
	FVector position;
	FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);

	offsetX = -(GRIDX / 2.0f) * GRIDSIZE;
	offsetY = -(GRIDY / 2.0f) * GRIDSIZE;

	i_playerLivesRemaining = 3;
	i_currentLevel = 1;
	

	// Build maze
	for (int i = 0; i < GRIDY; i++)
	{
		for (int j = 0; j < GRIDX; j++)
		{
			position = FVector(offsetX + (j * GRIDSIZE), offsetY + (i * GRIDSIZE), GRIDSIZE / 2.0f);

			if (maze[i][j] == N_GHOST && maze[i + 1][j] == N_GHOST)
			{
				// Special case - bars across ghost house
				position.X -= (GRIDSIZE * 0.25f);
				SpawnedWallPiece = GWorld->SpawnActor<AWallPiece>(AWallPiece::StaticClass(), position, rotation);
				SpawnedWallPiece->SetThin();
				position.X += (GRIDSIZE * 0.5f);
				position = FVector(offsetX + (j * GRIDSIZE) + (GRIDSIZE * 0.25f), offsetY + (i * GRIDSIZE), GRIDSIZE / 2.0f);
				SpawnedWallPiece = GWorld->SpawnActor<AWallPiece>(AWallPiece::StaticClass(), position, rotation);
				SpawnedWallPiece->SetThin();
			}
			if (maze[i][j] == N_WALL)
			{
				int adjacentSpaces = 0;
				bool north = false, east = false, south = false, west = false;
				bool northeast = false, northwest = false, southeast = false, southwest = false;
				if (i > 0 && maze[i - 1][j] > 0)
				{
					adjacentSpaces++;
					north = true;
				}
				if (i < GRIDY - 1 && maze[i + 1][j] > 0)
				{
					adjacentSpaces++;
					south = true;
				}
				if (j > 0 && maze[i][j - 1] > 0)
				{
					adjacentSpaces++;
					west = true;
				}
				if (j < GRIDX - 1 && maze[i][j + 1] > 0)
				{
					adjacentSpaces++;
					east = true;
				}
				if (i > 0 && j > 0 && maze[i - 1][j - 1] > 0)
				{
					northwest = true;
				}
				if (i > 0 && j < GRIDX - 1 && maze[i - 1][j + 1] > 0)
				{
					northeast = true;
				}
				if (i < GRIDY - 1 && j > 0 && maze[i + 1][j - 1] > 0)
				{
					southwest = true;
				}
				if (i < GRIDY - 1 && j < GRIDX - 1 && maze[i + 1][j + 1] > 0)
				{
					southeast = true;
				}

				if (adjacentSpaces == 0)
				{
					// Potential concave curves
					bool curve = northwest || northeast || southwest || southeast;
					if (curve)
					{
						if (northwest)
						{
							rotation.Yaw = 90.0f;
						}
						else if (northeast)
						{
							rotation.Yaw = 180.0f;
						}
						else if (southeast)
						{
							rotation.Yaw = 270.0f;
						}
						else
						{
							rotation.Yaw = 0.0f;
						}
						SpawnedWallPiece = GWorld->SpawnActor<AWallPiece>(AWallPiece::StaticClass(), position, rotation);
						SpawnedWallPiece->SetCurved(true);
					}
				}
				else if (adjacentSpaces == 1)
				{
					// Straight walls
					if (west || east)
					{
						rotation.Yaw = 90.0f;
					}
					else
					{
						rotation.Yaw = 0.0f;
					}
					SpawnedWallPiece = GWorld->SpawnActor<AWallPiece>(AWallPiece::StaticClass(), position, rotation);
				}
				else if (adjacentSpaces == 2)
				{
					// Convex curves
					if (north && east)
					{
						rotation.Yaw = 0.0f;
					}
					else if (east && south)
					{
						rotation.Yaw = 90.0f;
					}
					else if (south && west)
					{
						rotation.Yaw = 180.0f;
					}
					else
					{
						rotation.Yaw = 270.0f;
					}
					SpawnedWallPiece = GWorld->SpawnActor<AWallPiece>(AWallPiece::StaticClass(), position, rotation);
					SpawnedWallPiece->SetCurved(true);
				}
			}
		}
	}

	// Update maze array with node IDs and build basic navMesh
	i_nodeID = 0;
	navNode node;
	navMesh.Empty();
	for (int j = 0; j < 4; j++)
	{
		node.adjacent[j] = -1;
	}
	for (int i = 0; i < GRIDY; i++)
	{
		for (int j = 0; j < GRIDX; j++)
		{
			if (maze[i][j] > 0)
			{
				mazeNodes[i][j] = i_nodeID;
				i_nodeID++;
				node.x = j;
				node.y = i;
				node.pos = FVector(offsetX + (j * GRIDSIZE), offsetY + (i * GRIDSIZE), GRIDSIZE / 2.0f);
				node.slowNode = (maze[i][j] == N_SLOW ? true : false);
				node.ghostOnly = (maze[i][j] >= N_GHOST ? true : false);
				node.pill = NULL;
				navMesh.Add(node);
			}
			else
			{
				mazeNodes[i][j] = -1;
			}
		}
	}

	// Update navMesh with adjacent node info; find start nodes
	for (int i = 0; i < i_nodeID; i++)
	{
		int x = navMesh[i].x;
		int y = navMesh[i].y;
		int adjacentCount = 0;
		if (y < (GRIDY - 1))
		{
			navMesh[i].adjacent[SOUTH] = mazeNodes[y + 1][x];
			if (navMesh[i].adjacent[SOUTH] != -1)
			{
				adjacentCount++;
			}
		}
		if (y > 0)
		{
			navMesh[i].adjacent[NORTH] = mazeNodes[y - 1][x];
			if (navMesh[i].adjacent[NORTH] != -1)
			{
				adjacentCount++;
			}
		}
		if (x < (GRIDX - 1))
		{
			navMesh[i].adjacent[EAST] = mazeNodes[y][x + 1];
			if (navMesh[i].adjacent[EAST] != -1)
			{
				adjacentCount++;
			}
		}
		if (x > 0)
		{
			navMesh[i].adjacent[WEST] = mazeNodes[y][x - 1];
			if (navMesh[i].adjacent[WEST] != -1)
			{
				adjacentCount++;
			}
		}
		if (adjacentCount >= 3 && !navMesh[i].ghostOnly)
		{
			navMesh[i].isJunction = true;
		}
		else
		{
			navMesh[i].isJunction = false;
		}
		// Check for start nodes
		if (maze[y][x] == N_PACMAN) i_nodePacman = i;
		if (maze[y][x] == N_BLINKY) i_nodeBlinky = i;
		if (maze[y][x] == N_PINKY) i_nodePinky = i;
		if (maze[y][x] == N_INKY) i_nodeInky = i;
		if (maze[y][x] == N_CLYDE) i_nodeClyde = i;
	}



	// Calculate scatter nodes
	i_nodeScatterPinky = FindNearestNode(FVector(-GRIDX * GRIDSIZE, -GRIDY * GRIDSIZE, 0.0f));
	i_nodeScatterBlinky = FindNearestNode(FVector(GRIDX * GRIDSIZE, -GRIDY * GRIDSIZE, 0.0f));
	i_nodeScatterInky = FindNearestNode(FVector(GRIDX * GRIDSIZE, GRIDY * GRIDSIZE, 0.0f));
	i_nodeScatterClyde = FindNearestNode(FVector(-GRIDX * GRIDSIZE, GRIDY * GRIDSIZE, 0.0f));

	// Fix up special nodes
	i_nodeLeft = FindNearestNode(FVector(-GRIDX * GRIDSIZE, 0.0f, 0.0f));
	i_nodeRight = FindNearestNode(FVector(GRIDX * GRIDSIZE, 0.0f, 0.0f));
	navMesh[i_nodeLeft].adjacent[WEST] = i_nodeRight;
	navMesh[i_nodeRight].adjacent[EAST] = i_nodeLeft;

	// Spawn ghost
	ghostBlinky = GWorld->SpawnActor<AGhost>(AGhost::StaticClass(), navMesh[i_nodeBlinky].pos, rotation);
	ghostBlinky->SetBehaviour(BLINKY, i_nodeScatterBlinky);
	ghostPinky = GWorld->SpawnActor<AGhost>(AGhost::StaticClass(), navMesh[i_nodePinky].pos, rotation);
	ghostPinky->SetBehaviour(PINKY, i_nodeScatterPinky);
	ghostInky = GWorld->SpawnActor<AGhost>(AGhost::StaticClass(), navMesh[i_nodeInky].pos, rotation);
	ghostInky->SetBehaviour(INKY, i_nodeScatterInky);
	ghostClyde = GWorld->SpawnActor<AGhost>(AGhost::StaticClass(), navMesh[i_nodeClyde].pos, rotation);
	ghostClyde->SetBehaviour(CLYDE, i_nodeScatterClyde);

	// Spawn pacman - after the ghosts, as the Pacman class searches for all ghosts in the maze
	pacman = GWorld->SpawnActor<APacman>(APacman::StaticClass(), navMesh[i_nodePacman].pos, rotation);

	ResetPills();
	ResetMaze();
	SetGamePaused(true);
	
}

void AMain::EarthquakeEffect(float earthQuakeAmplitude, float earthQuakeDuration)
{
	float adjustedIntensity = b_accessibilityMode ? earthQuakeAmplitude / 4.0f : earthQuakeAmplitude;
	cameraController->EarthquakeEffect(adjustedIntensity, earthQuakeDuration);
}

void AMain::AddGhostTrap()
{
	if (i_amountOfGhostTrapsInLevel < MAXIMUM_GHOST_TRAPS)
	{
		i_amountOfGhostTrapsInLevel++;
	}
}

void AMain::RemoveGhostTrap()
{
	if (i_amountOfGhostTrapsInLevel > 1)
	{
		i_amountOfGhostTrapsInLevel--;
	}
}

void AMain::IncrementColourBlindnessMode()
{
	i_colourBlindnessMode++;
	if (i_colourBlindnessMode > MONOCHROMACY) { i_colourBlindnessMode = DEFAULT_VISION; }
	accessibilityUI->SetColourBlindnessMode(i_colourBlindnessMode);
	UIDisplay->UpdateAccessibilityMode(b_accessibilityMode, i_colourBlindnessMode);

}

void AMain::ResetColourBlindnessMode()
{
	i_colourBlindnessMode = DEFAULT_VISION;
	accessibilityUI->SetColourBlindnessMode(i_colourBlindnessMode);
	UIDisplay->UpdateAccessibilityMode(b_accessibilityMode, i_colourBlindnessMode);

}

void AMain::SetGamePaused(bool pausedState)
{
	pacman->SetPaused(pausedState);
	SetGhostPaused(pausedState);
}

void AMain::SetGhostPaused(bool pausedState)
{
	ghostBlinky->SetPaused(pausedState);
	ghostPinky->SetPaused(pausedState);

	if (!b_inkyIsReleased && i_maximumPills - i_pillsRemaining >= 30)
	{
		ghostInky->SetPaused(pausedState);
	}


	if (i_maximumPills != 0)
	{
		float percentagePillsConsumed = ((float)i_maximumPills - (float)i_pillsRemaining) / (float)i_maximumPills;
		if (!b_clydeIsReleased && percentagePillsConsumed > 0.333f)
		{
			ghostClyde->SetPaused(pausedState);
		}
	}
}

void AMain::ResetPills()
{
	i_pillsRemaining = 0;
	FVector position;
	FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < GRIDY; i++)
	{
		for (int j = 0; j < GRIDX; j++)
		{
			position = FVector(offsetX + (j * GRIDSIZE), offsetY + (i * GRIDSIZE), GRIDSIZE / 2.0f);

			if (maze[i][j] == N_PILL || maze[i][j] == N_POWERPILL)
			{
				if (!navMesh[mazeNodes[i][j]].pill)
				{
					navMesh[mazeNodes[i][j]].pill = GWorld->SpawnActor<APill>(APill::StaticClass(), position, rotation);
					if (maze[i][j] == N_POWERPILL)
					{
						navMesh[mazeNodes[i][j]].pill->SetPowerpill(true);
					}
				}
				i_pillsRemaining++;
			}
		}
	}
	i_maximumPills = i_pillsRemaining;
}

void AMain::ResetMaze()
{
	ghostBlinky->SetActorLocation(navMesh[i_nodeBlinky].pos);
	ghostBlinky->Reset();
	ghostPinky->SetActorLocation(navMesh[i_nodePinky].pos);
	ghostPinky->Reset();
	ghostInky->SetActorLocation(navMesh[i_nodeInky].pos);
	ghostInky->Reset();
	ghostInky->SetPaused(true);
	b_inkyIsReleased = false;
	ghostClyde->SetActorLocation(navMesh[i_nodeClyde].pos);
	ghostClyde->Reset();
	ghostClyde->SetPaused(true);
	b_clydeIsReleased = false;

	for (TObjectIterator<AGhostTrap> act; act; ++act)
	{
		act->Destroy();
	}

	i_amountOfGhostTrapsInLevel = 0;
	SetGhostPaused(false);
	pacman->SetActorLocation(navMesh[i_nodePacman].pos);
	pacman->Reset();
}

void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UIDisplay && pacman && !b_playerLost)
	{
		UIDisplay->UpdateCurrentPowerup(pacman);
	}
	
	bool resetNeeded = false;

	if (resetNeeded)
	{
		ResetMaze();
	}

}

int AMain::AdjacentNode(int from, int direction)
{
	return navMesh[from].adjacent[direction];
}

FVector AMain::GenerateRandomPoint()
{
	int randomIndex = rand() % (navMesh.Num() - 1);

	while (navMesh[randomIndex].ghostOnly)
	{
		randomIndex = rand() % (navMesh.Num() - 1);
	}
	return navMesh[randomIndex].pos;
}

void AMain::OnPlayerWin()
{
	i_currentLevel++;
	ShakeCamera(50, 0.125f);
	UIDisplay->UpdateCurrentLevel(i_currentLevel);
	i_pillsRemaining = i_maximumPills;
	ResetPills();
	ResetMaze();
}

void AMain::OnPlayerLose()
{
	SavingSystem saveObject;
	int currentHighScore = saveObject.LoadHighScore();
	if (currentHighScore < i_score){
		saveObject.SaveHighScore(i_score);
		UIDisplay->UpdateHighScore(i_score);
		
	}
	UIDisplay->OnPlayerLoseUI(i_score, currentHighScore);
	SetGamePaused(true);
	b_playerLost = true;
}

FVector AMain::GetNodePos(int node)
{
	return navMesh[node].pos;
}

int AMain::FindNearestNode(FVector pos, bool junction)
{
	float closestDist = (float)1e8;
	int closestNode = -1;

	for (int i = 0; i < navMesh.Num(); i++)
	{
		if (junction && !navMesh[i].isJunction)
		{
			continue;
		}
		FVector difference = pos - navMesh[i].pos;
		float distance = difference.Size2D();
		if (distance < closestDist)
		{
			closestDist = distance;
			closestNode = i;
		}
	}

	return closestNode;
}

int AMain::GetPacmanNode()
{
	if (pacman)
	{
		return pacman->CurrentNode();
	}
	else
	{
		return -1;
	}
}

APacman* AMain::GetPacman()
{
	return pacman;
}

AGhost* AMain::GetGhost(int ghostValue)
{
	switch (ghostValue)
	{
	case CLYDE:
	{
		return ghostClyde;
	}
	case INKY:
	{
		return ghostInky;
	}
	case BLINKY:
	{
		return ghostBlinky;
	}
	case PINKY:
	default:
	{
		return ghostPinky;
	}
	}
}

int AMain::GetScatterNode(int ghostValue) {
	switch (ghostValue)
	{
	case CLYDE:
	{
		return i_nodeScatterClyde;
	}
	case INKY:
	{
		return i_nodeScatterInky;
	}
	case BLINKY:
	{
		return i_nodeScatterBlinky;
	}
	case PINKY:
	default:
	{
		return i_nodeScatterPinky;
	}
	}
}

int AMain::GetStartingNode(int ghostValue) {
	switch (ghostValue)
	{
	case CLYDE:
	{
		return i_nodeClyde;
	}
	case INKY:
	{
		return i_nodeInky;
	}
	case BLINKY:
	{
		return i_nodeBlinky;
	}
	case PINKY:
	default:
	{
		return i_nodePinky;
	}
	}
}

int AMain::GetNodeDirection(int from, int to)
{
	int direction = -1;
	for (int i = 0; i < 4; i++)
	{
		if (navMesh[from].adjacent[i] == to)
		{
			direction = i;
			break;
		}
	}
	return direction;
}

void AMain::AddPoints(int points)
{
	//if the half points effect is applied, then all points added will be reduced by half
	bool halfPoints = pacman->GetRandomEffect() == EFFECT_HALFPOINTS;
	i_score += points / (halfPoints ? 2 : 1);
	UIDisplay->UpdateCurrentScore(i_score);
}

void AMain::ToggleScatterMode()
{
	b_scatter = !b_scatter;

	ghostBlinky->ToggleScatter();
	ghostPinky->ToggleScatter();
	ghostInky->ToggleScatter();
	ghostClyde->ToggleScatter();
}

void AMain::OnPowerPillConsumed() 
{
	ghostBlinky->SetFrightenedState();
	ghostInky->SetFrightenedState();
	ghostPinky->SetFrightenedState();
	ghostClyde->SetFrightenedState();
}

void AMain::EatPill(int node)
{
	APill* pill = navMesh[node].pill;

	if (pill)
	{
		if (pill->IsPowerpill())
		{
			ShakeCamera(15);
			OnPowerPillConsumed();
			SpawnParticles(pill, 10, SPECIAL_PARTICLE);
		}
		else
		{
			ShakeCamera(1, 0.125f);
			SpawnParticles(pill);
		}

		bool halfPointsEarned = pacman->GetRandomEffect() == EFFECT_HALFPOINTS;
		i_score += halfPointsEarned ? 5 : 10;

		i_pillsRemaining--;

		//continuously update the ghost states, depending on how many pills were eaten
		UpdateGhostStates();
		UIDisplay->UpdateCurrentScore(i_score);
		
		pill->Destroy();
		navMesh[node].pill = NULL;			
	}

	
}

void AMain::ToggleAccessibility() { 
	if (b_playerLost) { return; }
	b_accessibilityMode = !b_accessibilityMode; 

	UIDisplay->UpdateAccessibilityMode(b_accessibilityMode, i_colourBlindnessMode);
}

void AMain::UpdateGhostStates()
{
	// Inky is released after 30 pills are eaten
	if (!b_inkyIsReleased && i_maximumPills - i_pillsRemaining >= 30)
	{
		ghostInky->SetPaused(false);
		b_inkyIsReleased = true;
	}

	// Clyde is released when 1/3 of the pills are eaten
	if (!b_clydeIsReleased && (float)i_pillsRemaining / (float)i_maximumPills < 0.666f)
	{
		ghostClyde->SetPaused(false);
		b_clydeIsReleased = true;
	}
}

int AMain::EuclideanDistance(int from, int to)
{
	double xSquared = FMath::Square((navMesh[to].x - navMesh[from].x));
	double ySquared = FMath::Square((navMesh[to].y - navMesh[from].y));
	return FMath::Sqrt(xSquared + ySquared);
}

void AMain::SpawnParticles(AActor* actorPointer, int amount, int particleType)
{
	if (actorPointer == NULL) { return; }
	//do not spawn any particles if accessibility mode is enabled
	if (b_accessibilityMode) { return; }
		
	for (int i = 0; i < amount / 2; i++)
	{
		AParticleActor* particle = GWorld->SpawnActor<AParticleActor>(AParticleActor::StaticClass(), actorPointer->GetActorLocation(), actorPointer->GetActorRotation());
		if (particle) {
			particle->InitialiseMaterial(particleType);
		}
	}
}