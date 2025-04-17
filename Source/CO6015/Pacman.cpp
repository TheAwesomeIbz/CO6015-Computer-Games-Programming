// Fill out your copyright notice in the Description page of Project Settings.
#include "Pacman.h"
#include "Main.h"
#include "Ghost.h"
#include "UIDisplay.h"

// Sets default values
APacman::APacman()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	i_cachedDirection = -1;
	i_movingDirection = -1;
	f_randomEffectMode = -1;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	visualComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	visualComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Game/Meshes/SM_Pacman.SM_Pacman"));

	static ConstructorHelpers::FObjectFinder<UMaterial> yMaterial(TEXT("/Game/Materials/M_Yellow.M_Yellow"));
	static ConstructorHelpers::FObjectFinder<UMaterial> gMaterial(TEXT("/Game/Materials/M_Green.M_Green"));
	m_greenMaterial = gMaterial.Object;
	m_yellowMaterial = yMaterial.Object;

	if (meshAsset.Succeeded())
	{
		visualComponent->SetStaticMesh(meshAsset.Object);
		visualComponent->SetMaterial(0, yMaterial.Object);
		visualComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		visualComponent->SetWorldScale3D(FVector(PACMANSCALE));
	}
}

// Called when the game starts or when spawned
void APacman::BeginPlay()
{
	Super::BeginPlay();


	GameMode = (AMain*)(GetWorld()->GetAuthGameMode());
	
	f_powerUpParticleCounter = f_powerUpParticleDelay;
	i_cachedDirection = -1;
	i_movingDirection = -1;
	Reset();

	// Find all the ghosts in the level and add them
	ghosts.Empty();
	for (TObjectIterator<AGhost> act; act; ++act)
	{
		if (act->GetWorld() == this->GetWorld())
		{
			ghosts.Add(*act);
		}
		
	}
}

void APacman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Return out of the function if the player is paused
	if (b_paused) { return; }
	if (!GameMode) { return; }
	if (!GameMode->GameStarted()) { return; }


	float gameSpeedMultiplier = GameMode->IsAccessibilityModeEnabled() ? ACCESSIBILITY_MODE_GAME_SPEED : 1;
	RandomEffectTick(DeltaTime * gameSpeedMultiplier);
	MovePacmanTick(DeltaTime * gameSpeedMultiplier);
	RegisterGhostCollisionTick(DeltaTime * gameSpeedMultiplier);


	if (!GameMode->IsAccessibilityModeEnabled())
	{
		if (i_cachedDirection != -1)
			MoveNode(i_cachedDirection);
	}

}

void APacman::Reset()
{
	i_currentNode = GameMode->FindNearestNode(this->GetActorLocation());
	i_destinationNode = -1;
	i_previousNode = i_currentNode-1;
	b_isMoving = false;
	i_cachedDirection = -1;
	i_movingDirection = -1;
	f_randomEffectMode = -1;
	f_randomEffectTime = 0;
	visualComponent->SetMaterial(0, m_yellowMaterial);
	b_paused = false;
}

void APacman::RandomEffectTick(float _deltaTime)
{
	if (f_randomEffectTime >= 0)
	{
		f_randomEffectTime -= _deltaTime;
		visualComponent->SetMaterial(0, m_greenMaterial);


		f_powerUpParticleCounter -= _deltaTime;
		if (f_powerUpParticleCounter < 0)
		{
			f_powerUpParticleCounter = f_powerUpParticleDelay;
			GameMode->SpawnParticles(this, 2, PACMAN_RANDOMISED_PARTICLE);
		}
	}
	else
	{
		f_randomEffectMode = -1;
		visualComponent->SetMaterial(0, m_yellowMaterial);
	}
}

void APacman::MovePacmanTick(float _deltaTime)
{
	if (b_isMoving)
	{
		float localSpeedMultiplier = 1;
		if (f_randomEffectMode == EFFECT_SPEEDINCREASE) { localSpeedMultiplier = 1.5f; }
		else if (InvertedControls()) { localSpeedMultiplier = 2.0f; }
		else if (f_randomEffectMode == EFFECT_SPEEDDECREASE) { localSpeedMultiplier = 0.75f; }

		FVector currentPos = this->GetActorLocation();
		FVector deltaPos = v_movingTo - currentPos;
		float distToGo = deltaPos.Size();
		float distMax = _deltaTime * GRIDSIZE * PACMANSPEED * SPEEDMULTIPLIER * localSpeedMultiplier;	// How far Pacman will move this frame
		if (distToGo <= distMax)
		{
			// Reached a node
			b_isMoving = false;
			GameMode->EatPill(i_currentNode);	// Eat a pill if there is one in the node

			if (GameMode->AllPillsCollected())
			{
				GameMode->OnPlayerWin();
				return;
			}
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

void APacman::OnLifeLost()
{
	b_paused = true;
	GameMode->SpawnParticles(this, 20, HARMFUL_PARTICLE);
	GameMode->EarthquakeEffect(0, 0);

	if (GameMode->GetPlayerLives() > 1)
	{
		GameMode->ShakeCamera(15, 1);
		GameMode->ReducePlayerLives();


		for (int j = 0; j < ghosts.Num(); j++)
		{
			//stutter the game, but have a reset pending when the counter is over
			ghosts[j]->StutterGhost(true);
		}
	}
	else
	{
		GameMode->ShakeCamera(20, 1);
		GameMode->OnPlayerLose();
	}
}

void APacman::OnGhostEaten(AGhost* ghost)
{
	b_paused = true;

	//add points to pacman and stutter the game
	GameMode->AddPoints(400);
	GameMode->UIDisplay->SetPacmanPoints(GameMode->GetPacman(), f_randomEffectMode == EFFECT_HALFPOINTS ? 200 : 400);

	GameMode->ShakeCamera(10, 0.5f);
	ghost->OnEatenByPacman();

	for (int j = 0; j < ghosts.Num(); j++)
	{
		ghosts[j]->StutterGhost();
	}
}

void APacman::RegisterGhostCollisionTick(float _deltaTime)
{
	for (int i = 0; i < ghosts.Num(); i++)
	{
		FVector separation = this->GetActorLocation() - ghosts[i]->GetActorLocation();
		bool collidedWithGhost = separation.SizeSquared2D() < (COLLISION * COLLISION) && !ghosts[i]->IsEaten();
		if (collidedWithGhost && !ghosts[i]->IsPaused())
		{
			//if the ghost is frightened or frozen, then pacman can eat the ghost
			if (ghosts[i]->IsInFrightenedMode() ||
				ghosts[i]->IsFrozen())
			{
				OnGhostEaten(ghosts[i]);
				break;
			}
			OnLifeLost();
			
		}
	}
}

void APacman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveX", this, &APacman::MoveX);
	InputComponent->BindAxis("MoveY", this, &APacman::MoveY);

	InputComponent->BindAction("beginGame", IE_Pressed, this, &APacman::OnEnterButtonPressed);
	InputComponent->BindAction("toggleAccessibility", IE_Pressed, this, &APacman::OnControlButtonPressed);

}

void APacman::OnEnterButtonPressed()
{
	//if the game has started, then the enter button functions as input for when pacman loses the game
	//or as a way to toggle between different colour blindness modes.
	if (GameMode->GameStarted()) { 

		//warp to the original menu level scene
		if (GameMode->PlayerLost())
		{
			//code adapted from Unreal Engine (2016).
			UGameplayStatics::OpenLevel(this, TEXT("/Game/Menu/MenuLevel.MenuLevel'"), TRAVEL_Absolute);
			return;
		}
		if (GameMode->IsAccessibilityModeEnabled()) { GameMode->IncrementColourBlindnessMode(); }
		return;
	}

	//otherwise, initialise the game and its components
	GameMode->StartGame();
	GameMode->SetGamePaused(false);
	
	for (TObjectIterator<AUIDisplay> act; act; ++act){
		act->HideTitleUI();
		act->ShowGameUI();
	}
	
}

void APacman::OnControlButtonPressed()
{
	//if the game hasnt started or the player died, then don't allow the user to toggle the accessibility mode.
	if (!GameMode->GameStarted() || GameMode->PlayerLost()) { return; }

	GameMode->ToggleAccessibility();

	if (!GameMode->IsAccessibilityModeEnabled()) { GameMode->ResetColourBlindnessMode(); }

	//if the earthquake effect is applied, then disable the effect if the button is pressed.
	if (f_randomEffectMode == EFFECT_EARTHQUAKE)
	{
		GameMode->EarthquakeEffect(GameMode->IsAccessibilityModeEnabled() ? 0 : 15, GameMode->IsAccessibilityModeEnabled() ? 0 : (int)DurationLeft());
	}
}

void APacman::SetRandomEffect(int randomEffect)
{
	f_randomEffectTime = C_randomEffectDelay;
	f_randomEffectMode = randomEffect;

	if (randomEffect == EFFECT_FREEZEGHOST)
	{
		GameMode->GetGhost(INKY)->FreezeGhost();
		GameMode->GetGhost(BLINKY)->FreezeGhost();
		GameMode->GetGhost(CLYDE)->FreezeGhost();
		GameMode->GetGhost(PINKY)->FreezeGhost();
	}

	if (randomEffect == EFFECT_EARTHQUAKE){
		GameMode->EarthquakeEffect(20, 15);
	}
	else
	{
		GameMode->EarthquakeEffect(0, 0);
	}
}

bool APacman::MoveNode(int direction)
{
	if (b_isMoving || direction == -1)
	{
		return false;
	}

	int nodeCheck = GameMode->AdjacentNode(i_currentNode, direction);
	if (nodeCheck != -1 && !GameMode->IsGhostOnlyNode(nodeCheck))
	{
		MoveToNode(nodeCheck);
		return true;
	}
	else
	{
		return false;
	}
}

bool APacman::WallInDirection(int direction) {
	int nodeCheck = GameMode->AdjacentNode(i_currentNode, direction);
	return nodeCheck == -1;
}

void APacman::MoveToNode(int toNode)
{
	if (b_isMoving) { return; }
	i_movingDirection = GameMode->GetNodeDirection(i_currentNode, toNode);

	// Rotate Pacman to face the correct way when moving
	if (i_movingDirection == NORTH)
	{
		SetActorRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	}
	else if (i_movingDirection == EAST)
	{
		SetActorRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
	else if (i_movingDirection == SOUTH)
	{
		SetActorRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	}
	else
	{
		SetActorRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	}

	v_movingTo = GameMode->GetNodePos(toNode);

	// Teleport case across maze sides
	if ((i_currentNode == GameMode->GetLeftNode() && i_movingDirection == WEST) ||
		(i_currentNode == GameMode->GetRightNode() && i_movingDirection == EAST))
	{
		SetActorLocation(v_movingTo);
	}

	i_previousNode = i_currentNode;
	i_currentNode = toNode;
	b_isMoving = true;
}

void APacman::MoveX(float AxisValue)
{
	if (!GameMode->GameStarted()) { return; }

	if (GameMode->IsAccessibilityModeEnabled())
	{
		if (AxisValue > 0.0f)
		{
			MoveNode(EAST);
		}
		else if (AxisValue < 0.0f)
		{
			MoveNode(WEST);
		}
		return;
	}

	//Move function takes into consideration whether pacman has inverted controls or not
	//And adjusts the direction accordingly
	if (AxisValue > 0.0f && !WallInDirection(InvertedControls() ? WEST : EAST))
	{
		i_cachedDirection = !InvertedControls() ? EAST : WEST;
		
	}
	else if (AxisValue < 0.0f && !WallInDirection(InvertedControls() ? EAST : WEST))
	{
		i_cachedDirection = !InvertedControls() ? WEST : EAST;
		
	}
}

void APacman::MoveY(float AxisValue)
{
	if (!GameMode->GameStarted()) { return; }

	if (GameMode->IsAccessibilityModeEnabled())
	{
		if (AxisValue > 0.0f)
		{
			MoveNode(SOUTH);
		}
		else if (AxisValue < 0.0f)
		{
			MoveNode(NORTH);
		}
		return;
	}
	//Move function takes into consideration whether pacman has inverted controls or not
	//And adjusts the direction accordingly
	if (AxisValue > 0.0f && !WallInDirection(InvertedControls() ? NORTH : SOUTH))
	{
		i_cachedDirection = !InvertedControls() ? SOUTH : NORTH;
	}
	else if (AxisValue < 0.0f && !WallInDirection(InvertedControls() ? SOUTH : NORTH))
	{
		i_cachedDirection = !InvertedControls() ? NORTH : SOUTH;
	}
}

