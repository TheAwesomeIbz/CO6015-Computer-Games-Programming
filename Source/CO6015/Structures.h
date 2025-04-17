#pragma once

#include "Engine.h"
#define GRIDX (28)
#define GRIDY (31)
#define GHOST_FRIGHTENED_STATE_TIME (10)

#define ACCESSIBILITY_MODE_GAME_SPEED (0.65f)

//#define GRIDX (4)
//#define GRIDY (2)
#define GRIDSIZE (50.0f)
#define COLLISION (40.0f)
#define SCALE3DS (0.5f)

#define UPDATE_GHOST_PATH_FINDING (10.0f)

#define PACMANSPEED (5.5f)		// Pacman used a speed of 11 "tiles" per second - these were half the width used here
#define PACMANSCALE (0.6f)
#define GHOSTSPEED (5.5f)
#define GHOSTTELEPORTTIME (1.0f)
#define GHOSTSCALE (0.6f)
#define PILLSCALE (0.2f)
#define SPEEDMULTIPLIER (1.0f)


#define EFFECT_INVERTEDCONTROLS (1)

#define EFFECT_HALFPOINTS (2)

#define EFFECT_SPEEDINCREASE (3)
#define EFFECT_SPEEDDECREASE (4)
#define EFFECT_FREEZEGHOST (5)
#define EFFECT_EARTHQUAKE (6)
#define MAXIMUM_GHOST_TRAPS (20)

class APill;


enum
{
	NORTH = 0, SOUTH, EAST, WEST
};

enum
{
	BLINKY, PINKY, INKY, CLYDE		// Order of ghosts in original game - do not change
};

enum
{
	/// <summary>
	/// Particle for colliding with pills
	/// </summary>
	NORMAL_PARTICLE = 0, 
	
	/// <summary>
	/// Particle value for colliding with power pills or ghost traps
	/// </summary>
	SPECIAL_PARTICLE, 
	
	/// <summary>
	/// Particle value for colliding with ghosts to lose a life
	/// </summary>
	HARMFUL_PARTICLE, 
	
	/// <summary>
	/// Particle value for when pacman has a random effect applied
	/// </summary>
	PACMAN_RANDOMISED_PARTICLE
};

enum
{
	DEFAULT_VISION, DEUTERANOPIA, PROTANOPIA, TRITANOPIA, MONOCHROMACY
};

struct navNode
{
	int x, y;
	int adjacent[4];	// uses the enum above to hold adjacent node ids by cardinals
	bool isJunction;
	bool ghostOnly;
	bool slowNode;
	FVector pos;
	APill* pill;
};


/// <summary>
/// Code adapted from Tarodev (2021). Node class used for A* pathfinding within the Ghost Class. 
/// </summary>
class PathNode
{
public:

	int i_currentNodeValue;
	int i_previousNodeValue;
	int GCost;
	int HCost;


	PathNode(int connID) {
		i_currentNodeValue = connID;
		i_previousNodeValue = -1;
		GCost = -1;
		HCost = -1;
	}

	/// <returns>The actual cost of the path from the G and H cost</returns>
	int GetFCost() const { return this->GCost + this->HCost; }
	int GetGCost() const { return this->GCost; }
	int GetHCost() const { return this->HCost; }

	void SetHCost(int hcost) { this->HCost = hcost; }
	void SetGCost(int gcost) { this->GCost = gcost; }

	int GetCurrentNodeValue() { return i_currentNodeValue; }

	/// <returns>The connected node to the current one, i.e. the previous node.</returns>
	int GetConnectedNode() { return i_previousNodeValue; }

	void SetPreviousNodeValue(int id) { this->i_previousNodeValue = id; }

	/// <summary>
	/// Operator overload that determine whether two node values are equal. There were issues with comparing references, so the alternative was to compare IDs for each path node.
	/// </summary>
	/// <param name="lhs"></param>
	/// <param name="rhs"></param>
	/// <returns>Whether the two node values are equal</returns>
	friend bool operator==(const PathNode& lhs, const PathNode& rhs) { return lhs.i_currentNodeValue == rhs.i_currentNodeValue; }
};

