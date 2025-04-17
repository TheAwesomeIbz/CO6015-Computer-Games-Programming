// Fill out your copyright notice in the Description page of Project Settings.


#include "Ghost.h"
#include "Main.h"
#include "Pacman.h"
#include "GhostTrap.h"
#include "Engine.h"

AGhost::AGhost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Initialise root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	visualComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	visualComponent->SetupAttachment(RootComponent);

	InitialiseMaterials();

	i_ReverseDestinationNode = -1;
	PathFindingNodes = TArray<PathNode>();
	

}

void AGhost::BeginPlay()
{
	Super::BeginPlay();
	f_trapTimer = f_trapDelay;
	f_ghostFrozenTimer = 0;
	GameMode = (AMain*)(GetWorld()->GetAuthGameMode());
	i_behaviour = BLINKY;	
	b_IsEaten = false;
	Reset();
}

void AGhost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float gameDeltaTime = DeltaTime * (GameMode->IsAccessibilityModeEnabled() ? ACCESSIBILITY_MODE_GAME_SPEED : 1);

	if (f_ghostStutterCounter > 0 && f_ghostStutterCounter != -1)
	{
		f_ghostStutterCounter -= gameDeltaTime;


	}

	if (f_ghostStutterCounter < 0 && f_ghostStutterCounter != -1)
	{
		f_ghostStutterCounter = -1;
		GameMode->GetPacman()->SetPaused(false);
		GameMode->UIDisplay->SetPacmanPoints(NULL, 0);
		b_ghostPaused = false;

		if (b_stutterResetPending)
		{
			GameMode->UIDisplay->UpdateLives(GameMode->GetPlayerLives());
			GameMode->ResetMaze();
			b_stutterResetPending = false;
		}
	}

	if (b_ghostPaused) return;
	if (GameMode->PlayerLost()) return;





	if (f_ghostFrozenTimer > 0)
	{

		visualComponent->SetRelativeLocation(FVector(0, FMath::Sin(f_ghostFrozenTimer * 64.0f) * 10.0f * (f_ghostFrozenTimer / 4), 0));
		f_ghostFrozenTimer -= gameDeltaTime;
	}
	else
	{
		visualComponent->SetRelativeLocation(FVector(0));
	}

	//if ghost is frozen, halt all functionality and set the material to a frozen material
	if (IsFrozen()) {
		visualComponent->SetMaterial(0, frozenMaterial);
		return;
	}


	TrapTimer(gameDeltaTime);
	f_currentFrightenedTime -= f_currentFrightenedTime >= 0 ? gameDeltaTime : 0;

	if (!IsInFrightenedMode() && !b_IsEaten)
	{
		f_modeTimer -= gameDeltaTime * SPEEDMULTIPLIER;
		if (f_modeTimer < 0.0f)
		{
			ToggleScatter();
		}
	}

	GhostMovementTick(gameDeltaTime);

	if (IsInFrightenedMode() && !b_IsEaten)
	{
		FrightenedPhase();
		return;
	}

	if (b_IsEaten && i_currentNode != GameMode->GetStartingNode(i_behaviour))
	{
		visualComponent->SetMaterial(0, eatenMaterial);
		i_destinationNode = GameMode->GetStartingNode(i_behaviour);
		TArray<PathNode> pathNodes = AStarPathFinding(i_currentNode, i_destinationNode, true);
		TravelToDestination(pathNodes);
		return;
	}

	visualComponent->SetMaterial(0, materials[i_behaviour]);

	if (b_IsEaten && GameMode->IsGhostOnlyNode(i_currentNode))
	{
		b_IsEaten = false;
		f_currentFrightenedTime = 0;
	}


	GhostTargetCalculationTick(gameDeltaTime);

}

void AGhost::InitialiseMaterials()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Game/Meshes/SM_Ghost.SM_Ghost"));
	static ConstructorHelpers::FObjectFinder<UMaterial> redM(TEXT("/Game/Materials/M_Red.M_Red"));
	static ConstructorHelpers::FObjectFinder<UMaterial> pinkM(TEXT("/Game/Materials/M_Pink.M_Pink"));
	static ConstructorHelpers::FObjectFinder<UMaterial> blueM(TEXT("/Game/Materials/M_LightBlue.M_LightBlue"));
	static ConstructorHelpers::FObjectFinder<UMaterial> orangeM(TEXT("/Game/Materials/M_Orange.M_Orange"));
	static ConstructorHelpers::FObjectFinder<UMaterial> purpleM(TEXT("/Game/Materials/M_Purple.M_Purple"));
	static ConstructorHelpers::FObjectFinder<UMaterial> eatenM(TEXT("/Game/Materials/M_Grey.M_Grey"));

	static ConstructorHelpers::FObjectFinder<UMaterial> frozenM(TEXT("/Game/Materials/M_LightCyan.M_LightCyan"));

	materials[0] = redM.Object;
	materials[1] = pinkM.Object;
	materials[2] = blueM.Object;
	materials[3] = orangeM.Object;

	frightenedMaterial = purpleM.Object;
	eatenMaterial = eatenM.Object;
	frozenMaterial = frozenM.Object;

	if (meshAsset.Succeeded())
	{
		visualComponent->SetStaticMesh(meshAsset.Object);
		visualComponent->SetMaterial(0, materials[0]);		// Default
		visualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		visualComponent->SetWorldScale3D(FVector(GHOSTSCALE));
	}
}

void AGhost::TrapTimer(float _deltaTime)
{
	f_trapTimer -= _deltaTime;

	if (f_trapTimer < 0)
	{
		f_trapTimer = f_trapDelay - (rand() % 5);
		if (!b_ghostPaused && !GameMode->IsGhostOnlyNode(i_currentNode))
		{
			int ghostTrapsExisting = 0;
			if (GameMode->CanAddGhostTrap())
			{
				GameMode->AddGhostTrap();
				AGhostTrap* ghostTrap = GWorld->SpawnActor<AGhostTrap>(AGhostTrap::StaticClass(), GameMode->GenerateRandomPoint(), GetActorRotation());
			}
			

			
		}
	}

	
}

void AGhost::Reset()
{
	i_currentNode = GameMode->FindNearestNode(this->GetActorLocation());
	i_destinationNode = -1;
	i_previousNode = -1;
	b_isMoving = false;
	i_movingDirection = NORTH;
	b_iscattering = true;
	f_modeTimer = 7.0f;
	
	b_reversePending = false;
	b_ghostPaused = false;
	i_ReverseDestinationNode = -1;
	b_IsEaten = false;

	f_currentFrightenedTime = 0.0F;
	visualComponent->SetMaterial(0, materials[i_behaviour]);
}

void AGhost::SetFrightenedState()
{
	f_currentFrightenedTime = GHOST_FRIGHTENED_STATE_TIME;
	b_iscattering = false;
	int reverseDirection;
	switch (i_movingDirection)
	{
	case NORTH:
		reverseDirection = SOUTH;
		break;
	case SOUTH:
		reverseDirection = NORTH;
		break;
	case EAST:
		reverseDirection = WEST;
		break;
	case WEST: default:
		reverseDirection = EAST;
		break;
	}

	//set the adjacent node to node in the reverse direction
	int node = GameMode->AdjacentNode(i_currentNode, reverseDirection);

	if (node != -1) {
		i_ReverseDestinationNode = node;
	}
}

void AGhost::OnEatenByPacman()
{
	b_IsEaten = true;
	f_ghostFrozenTimer = -1;
	GameMode->SpawnParticles(this, 10, SPECIAL_PARTICLE);
}

void AGhost::StutterGhost(bool resetPending)
{
	
	b_ghostPaused = true;
	f_ghostStutterCounter = f_ghostStutterDelay;
	b_stutterResetPending = resetPending;
}

void AGhost::GhostMovementTick(float _deltaTime)
{
	if (b_isMoving)
	{
		FVector currentPos = this->GetActorLocation();
		FVector deltaPos = v_movingTo - currentPos;
		float distToGo = deltaPos.Size();

		float pillConsumedMultiplier = GameMode->HalfOfPillsConsumed() ? 1.125f : 1;
		float distMax = _deltaTime * GRIDSIZE * GHOSTSPEED * SPEEDMULTIPLIER * pillConsumedMultiplier * (IsEaten() ? i_EatenGhostSpeedMultiplier : 1);
		if (GameMode->IsSlowNode(i_currentNode))
		{
			distMax /= 2.0f;
		}
		if (distToGo <= distMax)
		{
			b_isMoving = false;
		}
		else
		{
			distToGo = distMax;
		}
		deltaPos.Normalize();
		deltaPos *= distToGo;
		this->SetActorLocation(currentPos + deltaPos);
	}
}

void AGhost::GhostTargetCalculationTick(float _deltaTime)
{
	if (IsFrozen()) return;
	if (b_isMoving) return;

	FVector pacmanCoordinate = GameMode->GetPacman()->GetActorLocation();

	// If they are set to scatter they head towards their designated corner
	if (b_iscattering)
	{
		i_destinationNode = i_scatterNode;
		SimpleGhostTrackingAlgorithm();
	}
	else switch (i_behaviour)
	{
		//Pinky will always chase 4 tiles in front of Pacman, except when pacman is facing north
		//Pinky will chase 4 tiles ahead and 4 tiles ahead of Pacman
		//If neither of those tiles exist, pinky will chase the 4nearest tile to that position
	case PINKY:
	{
		i_destinationNode = GameMode->FindNearestNode(GetTargetTile(4));
		break;
	}

	//Clyde will track pacman directly only if clyde is 8 or more tiles away from pacman,
	//Else, clyde will behave as if they were in scatter mode
	case CLYDE:
	{
		FVector clydePosition = GameMode->GetGhost(CLYDE)->GetActorLocation();
		FVector pacManPosition = GameMode->GetPacman()->GetActorLocation();

		float squareMagnitude = FMath::Square((pacManPosition.X - clydePosition.X) / GRIDSIZE) + FMath::Square((pacManPosition.Y - clydePosition.Y) / GRIDSIZE);

		i_destinationNode = squareMagnitude < 64 ?
			GameMode->GetScatterNode(CLYDE) : GameMode->GetPacmanNode();

		break;
	}

	//An intermediate tile is found; (2 tiles in front of pacman or nearest tile)
	//Find the vector of this point to Blinky, and subtract it from the intermediate tile
	//find the nearest node to this tile, and this will be inky's target tile
	case INKY:
	{
		FVector intermediatePosition = GetTargetTile(2);
		FVector blinkyPosition = GameMode->GetGhost(BLINKY)->GetActorLocation();
		i_destinationNode = GameMode->FindNearestNode(((blinkyPosition - intermediatePosition) / GRIDSIZE) * 2);
	}

	//Blinky or any default ghost will always follow Pacman's current node position
	case BLINKY:
	default:
	{
		i_destinationNode = GameMode->GetPacmanNode();
		break;
	}
	}

	if (!b_iscattering)
	{
		ChasePlayerPhase(_deltaTime);
	}

}

void AGhost::SimpleGhostTrackingAlgorithm(bool _randomiseMovement) {
	int nodeCheck = -1;

	// Is this ghost still in the ghost house? If so, go North if possible to exit
	if (GameMode->IsGhostOnlyNode(i_currentNode))
	{
		nodeCheck = GameMode->AdjacentNode(i_currentNode, NORTH);
		if (nodeCheck == -1)
		{
			//this means we need to move horizontally
			int adjacentEastNode = GameMode->AdjacentNode(i_currentNode, EAST);
			nodeCheck = adjacentEastNode != -1 ? adjacentEastNode : GameMode->AdjacentNode(i_currentNode, WEST);
		}

		MoveToNode(nodeCheck);
	}

	// If we are not at a junction keep moving forward if possible, otherwise turn the corner
	if (!GameMode->IsJunctionNode(i_currentNode))
	{
		nodeCheck = GameMode->AdjacentNode(i_currentNode, i_movingDirection);

		if (nodeCheck != -1)
		{
			MoveToNode(nodeCheck);
			return;
		}
		
		// Couldn't move forward, so must be on a corner, so find the exit direction
		for (int i = 0; i < 4; i++)
		{
			nodeCheck = GameMode->AdjacentNode(i_currentNode, i);
			if (nodeCheck != -1 && nodeCheck != i_previousNode)
			{
				MoveToNode(nodeCheck);
				return;
			}
		}
	}

	if (_randomiseMovement)
	{
		int randomExit = -1;

		while (randomExit == -1)
		{
			int randomDirection = (rand() % 4);

			if (TravellingBackwards(randomDirection)) continue;

			randomExit = GameMode->AdjacentNode(i_currentNode, randomDirection);
			if (randomExit != -1 && GameMode->IsGhostOnlyNode(randomExit)) {
				randomExit = -1;
			}
		}

		MoveToNode(randomExit);
	}
	else
	{
		int bestExit = -1;
		int bestDist = 1e6;
		int currentDist = GameMode->EuclideanDistance(i_currentNode, i_destinationNode);

		for (int i = 3; i >= 0; i--)
		{
			nodeCheck = GameMode->AdjacentNode(i_currentNode, i);
			if (nodeCheck == -1) continue;
			if (TravellingBackwards(i)) continue;
			if (GameMode->IsGhostOnlyNode(nodeCheck) && !b_IsEaten) continue;

			int thisExitDist = GameMode->EuclideanDistance(nodeCheck, i_destinationNode);
			if (thisExitDist <= bestDist)
			{
				bestExit = nodeCheck;
				bestDist = thisExitDist;
			}
		}

		MoveToNode(bestExit);
	}

	
}

void AGhost::FrightenedPhase()
{
	visualComponent->SetMaterial(0, frightenedMaterial);

	if (i_currentNode == i_ReverseDestinationNode)
	{
		i_ReverseDestinationNode = -1;
	}

	if (i_ReverseDestinationNode != -1)
	{
		MoveToNode(i_ReverseDestinationNode);
		return;
	}

	SimpleGhostTrackingAlgorithm(true);
}

bool AGhost::TravellingBackwards(int _targetDirection)
{
	switch (i_movingDirection)
	{
	case NORTH:
	{
		return _targetDirection == SOUTH;
	}
	case SOUTH:
	{

		return _targetDirection == NORTH;
	}
	case EAST:
	{
		return _targetDirection == WEST;
	}
	case WEST:
	{
		return _targetDirection == EAST;
	}
	default:
	{
		return true;
	}

	}
}

void AGhost::ChasePlayerPhase(float _deltaTime)
{
	PathFindingNodes = AStarPathFinding(i_currentNode, i_destinationNode);
	TravelToDestination(PathFindingNodes);
}

void AGhost::TravelToDestination(TArray<PathNode> pathNodes) {

	if (pathNodes.Num() > 1)
	{
		for (int i = pathNodes.Num() - 1; i > 0; i--)
		{
			if (i_currentNode == pathNodes[i].i_currentNodeValue)
			{
				int adjacentIndex = i - 1 < 0 ? 0 : i - 1;
				MoveToNode(pathNodes[adjacentIndex].i_currentNodeValue);
			}
		}
	}
	else
	{
		SimpleGhostTrackingAlgorithm();
	}
}

void AGhost::MoveToNode(int toNode)
{
	if (b_isMoving) return;

	i_movingDirection = GameMode->GetNodeDirection(i_currentNode, toNode);

	v_movingTo = GameMode->GetNodePos(toNode);

	// Teleport case across maze sides
	if ((i_currentNode == GameMode->GetLeftNode() && toNode == GameMode->GetRightNode()) ||
		(i_currentNode == GameMode->GetRightNode() && toNode == GameMode->GetLeftNode()))
	{
		SetActorLocation(v_movingTo);
	}

	i_previousNode = i_currentNode;
	i_currentNode = toNode;
	b_isMoving = true;
}

void AGhost::SetBehaviour(int newBehaviour, int newNode)
{
	i_behaviour = newBehaviour;
	i_scatterNode = newNode;
	visualComponent->SetMaterial(0, materials[i_behaviour]);
}

void AGhost::ToggleScatter()
{
	// The Ghosts in Pacman scatter every now and then to make the game playable
	// This has a pattern you can research, but for now, they will scatter for 10 seconds, then
	// chase the player for 20 seconds and repeat this forever

	b_iscattering = !b_iscattering;

	if (b_iscattering)
	{
		f_modeTimer = 10.0f;
	}
	else
	{
		f_modeTimer = 20.0f;
	}
	

	// Ghosts reverse direction at the next junction when switching between scattering and not
	b_reversePending = true;
}

FVector AGhost::GetTargetTile(int _displacementFactor)
{
	switch (GameMode->GetPacman()->MovingDirection())
	{
	case NORTH:
	{
		return GameMode->GetPacman()->GetActorLocation() + FVector(_displacementFactor * GRIDSIZE, _displacementFactor * GRIDSIZE, 0);
	}
	case SOUTH:
	{
		return GameMode->GetPacman()->GetActorLocation() - FVector(_displacementFactor * GRIDSIZE, 0, 0);
	}
	case EAST:
	{
		return GameMode->GetPacman()->GetActorLocation() + FVector(_displacementFactor * GRIDSIZE, 0, 0);
	}
	case WEST:
	default:
	{
		return GameMode->GetPacman()->GetActorLocation() - FVector(_displacementFactor * GRIDSIZE, 0, 0);
	}
	}
}

TArray<PathNode> AGhost::FindAllNeighbours(PathNode curr, bool _scanNodeBehind)
{
	TArray<PathNode> neighbours;
	int currentDirection = i_movingDirection;
	for (int i = 0; i < 4; i++)
	{
		int nodeCheck = GameMode->AdjacentNode(curr.GetCurrentNodeValue(), i);
		bool backwardsTravel = _scanNodeBehind ? true : !TravellingBackwards(i);

		if (nodeCheck != -1 && backwardsTravel)
		{
			PathNode newNode(nodeCheck);
			newNode.SetPreviousNodeValue(curr.i_currentNodeValue);
			neighbours.Add(newNode);
		}
	}
	return neighbours;
}

TArray<PathNode> AGhost::AStarPathFinding(int startNodeValue, int endNodeValue, bool canTravelBackwards)
{
	//Code translated to c++, but logic is based on code adapted from Tarodev (2021).
	PathNode startNode(startNodeValue);
	PathNode targetNode(endNodeValue);

	//two lists, one for searching and processed nodes
	TArray<PathNode> toSearchNodes;
	toSearchNodes.Add(startNode);
	TArray<PathNode> processedNodes;

	//if something exists in the to search list, set the current node to the first item in the array
	while (!toSearchNodes.IsEmpty())
	{
		//loop through each other items in toSearch, and see if the f cost is less than the current
		//F cost, or equal to the current f cost but a lower H cost.If that is the case, set Current to the node with
		//The smallest value that satisfies these conditions

		PathNode current = toSearchNodes[0];
		for (int i = 1; i < toSearchNodes.Num(); i++)
		{
			PathNode currentSearchNodeIndex = toSearchNodes[i];
			if (currentSearchNodeIndex.GetFCost() < current.GetFCost() || currentSearchNodeIndex.GetFCost() == current.GetFCost() && currentSearchNodeIndex.GetHCost() < current.GetHCost())
			{
				current = currentSearchNodeIndex;
			}
		}

		//after iteration, add current to processed and remove current from tosearch, to search for another element
		processedNodes.Add(current);
		toSearchNodes.Remove(current);

		//if the target node has been reached, then return the path by retracing the connections within each node until the start node is reached
		//if the count exceeds 100, that means something else has gone wrong, so default to travelling in a random direction otherwise

		if (current == targetNode)
		{
			TArray<PathNode> path;
			PathNode currentPathNode = current;

			path.Add(currentPathNode);
			while (currentPathNode.i_previousNodeValue != -1)
			{
				PathNode existingPathNode(-1);
				for (int i = 0; i < processedNodes.Num(); i++)
				{
					if (currentPathNode.i_previousNodeValue == processedNodes[i].i_currentNodeValue)
					{
						currentPathNode = processedNodes[i];
					}
				}

				path.Add(currentPathNode);

			}
			return path;
		}

		//Find all neighbours of nodes, by scanning for all directional nodes and checking whether they are walkable and not already processed
		TArray<PathNode> allNeighbours = FindAllNeighbours(current, canTravelBackwards);
		for (int i = 0; i < allNeighbours.Num(); i++)
		{
			bool nodeInSearch = false;
			for (int j = 0; j < toSearchNodes.Num(); j++)
			{
				nodeInSearch = allNeighbours[i] == toSearchNodes[j];
				if (nodeInSearch) { break; }
			}

			PathNode neighbour = allNeighbours[i];
			int movementCostToNeighbour = current.GetGCost() + GameMode->EuclideanDistance(current.GetCurrentNodeValue(), neighbour.GetCurrentNodeValue());

			if (processedNodes.Contains(neighbour))
			{
				continue;
			}

			//if the neighbour hasn’t been searched or the cost to the neighbour is smaller than its g cost, then set
			//The neighbours g cost and connection to current node
			if (!nodeInSearch || movementCostToNeighbour < neighbour.GetGCost())
			{
				neighbour.SetGCost(movementCostToNeighbour);
				neighbour.SetPreviousNodeValue(current.i_currentNodeValue);

				//if the neighbour hasn’t been searched through
				//then set the h cost to the Manhattan distance to the target node
				//Add the neighbour to search and search through the neighbour’s neighbour
				if (!nodeInSearch)
				{
					int euclideanDistance = GameMode->EuclideanDistance(neighbour.GetCurrentNodeValue(), targetNode.GetCurrentNodeValue());
					neighbour.SetHCost(euclideanDistance);
					toSearchNodes.Add(neighbour);
				}
			}

		}


	}


	return TArray<PathNode>();
}