// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <iostream>
#include <fstream>
#include "CoreMinimal.h"

/// <summary>
/// Class to handle saving and loading of high scores
/// </summary>
class CO6015_API SavingSystem
{
public:
	SavingSystem();

	/// <summary>
	/// Save the current highscore to a file
	/// </summary>
	/// <param name="highScore"></param>
	void SaveHighScore(int highScore);

	/// <returns>The current high score saved locally to disk</returns>
	int LoadHighScore();

	~SavingSystem();
};
