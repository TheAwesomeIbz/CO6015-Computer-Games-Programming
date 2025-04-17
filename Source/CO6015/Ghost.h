#pragma once

#include "Engine.h"
#include "Structures.h"
#include "Ghost.generated.h"

class AMain;

/// <summary>
/// Actor class that represents the ghost entity in the game
/// </summary>
UCLASS()
class CO6015_API AGhost : public AActor
{
	GENERATED_BODY()
	
private:
	virtual void BeginPlay() override;

	UStaticMeshComponent* visualComponent;
	
	int i_ReverseDestinationNode;

	bool b_IsEaten;
	float f_trapTimer;
	const float f_trapDelay = 5.0f;

	float f_ghostFrozenTimer;
	const float f_ghostFrozenDelay = 5.0f;

	float f_ghostStutterCounter;
	const float f_ghostStutterDelay = 1.0f;

	const int i_EatenGhostSpeedMultiplier = 3;
	
	float f_currentFrightenedTime;

	int i_currentNode;
	int i_previousNode;
	bool b_isMoving;
	FVector v_movingTo;
	int i_destinationNode;
	int i_movingDirection;
	int i_scatterNode;
	bool b_iscattering;
	float f_modeTimer;
	bool b_reversePending;
	bool b_ghostPaused;
	bool b_stutterResetPending;

	UMaterial* materials[4];
	UMaterial* frightenedMaterial;
	UMaterial* eatenMaterial;
	UMaterial* frozenMaterial;

	AMain* GameMode;
	int i_behaviour;

	/// <summary>
	/// Instance of nodes to track
	/// </summary>
	TArray<PathNode> PathFindingNodes;

	/// <summary>
	/// Method called in constructor to initialise the materials for the ghosts
	/// </summary>
	void InitialiseMaterials();

	/// <summary>
	/// Frightened phase of the ghost where the ghosts run away from the player
	/// </summary>
	void FrightenedPhase();

	/// <summary>
	/// Update method called in tick to update the movement of the ghost
	/// </summary>
	/// <param name="_deltaTime"></param>
	void GhostMovementTick(float _deltaTime);

	/// <summary>
	/// Update method called in tick to update the calculation of the ghost target
	/// </summary>
	/// <param name="_deltaTime"></param>
	void GhostTargetCalculationTick(float _deltaTime);

	/// <summary>
	/// Called on tick to update the trap timer
	/// </summary>
	/// <param name="_deltaTime"></param>
	void TrapTimer(float _deltaTime);

	/// <summary>
	/// Method that looks at the current path node and finds all the adjacent nodes that can be travelled to. This output also depends whether the ghost can travel backwards or not
	/// </summary>
	/// <param name="curr">: Current path node</param>
	/// <param name="_scanNodeBehind">: Whether the algorithm can find the shortest path travelling backwards</param>
	/// <returns>An array of path nodes that contain all neighbours to current node</returns>
	TArray<PathNode> FindAllNeighbours(PathNode curr, bool _scanNodeBehind = false);

	/// <summary>
	/// Code adapted from Tarodev C# video tutorial explanation: Pathfinding – Understanding A* (2021)
	/// This methods creates an A* pathfinding algorithm to find the shortest path between two nodes. This algorithm can consider the direction of the ghost and whether it can travel backwards.
	/// </summary>
	/// <param name="startNode">: Starting node of algorithm</param>
	/// <param name="endNode">: Ending node of algorithm</param>
	/// <param name="canTravelBackwards">: Whether the algorithm can find the shortest path travelling backwards</param>
	/// <returns>An array of PathNode objects that contain the path in which an entity should trace</returns>
	TArray<PathNode> AStarPathFinding(int startNode, int endNode, bool canTravelBackwards = false);

	/// <summary>
	/// Travel across the path nodes in the array to reach the destination. 
	/// </summary>
	/// <param name="pathNodes"></param>
	void TravelToDestination(TArray<PathNode> pathNodes);

	/// <summary>
	/// Default ghost tracking algorithm used to find the shortest euclidean distance to the target using euclidean distance
	/// </summary>
	void SimpleGhostTrackingAlgorithm(bool _randomised = false);

	/// <summary>
	/// The A* pathfinding algorithm to chase the player implementation
	/// </summary>
	/// <param name="_deltaTime"></param>
	void ChasePlayerPhase(float _deltaTime);

	/// <summary>
	/// Determine whether the ghost is travelling backwards
	/// </summary>
	/// <param name="_targetDirection"></param>
	/// <returns>Whether the ghost is travelling backwards</returns>
	bool TravellingBackwards(int _targetDirection);

	/// <summary>
	/// Move the ghost to the nearest adjacent node
	/// </summary>
	/// <param name="toNode"></param>
	void MoveToNode(int toNode);

	/// <summary>
	/// Get the target tile of the ghost depending on the displacement factor
	/// </summary>
	/// <param name="_displacementFactor"></param>
	/// <returns></returns>
	FVector GetTargetTile(int _displacementFactor);

public:	
	// Sets default values for this actor's properties
	AGhost();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/// <returns>Whether the ghost is in frightened mode</returns>
	bool IsInFrightenedMode() { return f_currentFrightenedTime > 0; }

	/// <returns>Whether the ghost is paused</returns>
	bool IsPaused() { return b_ghostPaused; }
		
	void Reset();

	/// <summary>
	/// Stutter the game when the ghost is eaten
	/// </summary>
	/// <param name="resetPending"></param>
	void StutterGhost(bool resetPending = false);
	
	/// <summary>
	/// Set the behaviour of the ghost
	/// </summary>
	/// <param name="newBehaviour"></param>
	/// <param name="newNode"></param>
	void SetBehaviour(int newBehaviour, int newNode);

	/// <returns>The current behaviour of the ghost</returns>
	int GetBehaviour() { return i_behaviour; }

	/// <returns>Whether the ghost is scattering or not</returns>
	bool IsScattering() { return b_iscattering; }

	/// <summary>
	/// Toggle the scatter mode of the ghost
	/// </summary>
	void ToggleScatter();

	/// <summary>
	/// Set the state of the ghost to be paused
	/// </summary>
	/// <param name="isPaused"></param>
	void SetPaused(bool isPaused) { b_ghostPaused = isPaused; }

	/// <returns>Returns whether the Ghost is eaten</returns>
	bool IsEaten() { return b_IsEaten; }

	/// <summary>
	/// Set the frightened state of the ghost depending on its current moving direction
	/// </summary>
	void SetFrightenedState();

	/// <summary>
	/// Called when the ghost is eaten by pacman
	/// </summary>
	void OnEatenByPacman();

	/// <summary>
	/// Set the ghost to be frozen
	/// </summary>
	void FreezeGhost() { f_ghostFrozenTimer = f_ghostFrozenDelay;}

	/// <returns>Whether the ghost is frozen or not</returns>
	bool IsFrozen() { return f_ghostFrozenTimer > 0; }

	
};
