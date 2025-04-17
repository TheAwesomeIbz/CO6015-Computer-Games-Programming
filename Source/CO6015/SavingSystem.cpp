// Fill out your copyright notice in the Description page of Project Settings.

#include "SavingSystem.h"
#include <filesystem>


SavingSystem::SavingSystem()
{

}

void SavingSystem::SaveHighScore(int highScore)
{
	//code adapted from Cplusplus.com (2023)
	std::ofstream file;

	//used to truncate a file's content when it is opened. This means that if the file already exists, 
	// its existing content will be deleted, and the file will start with a size of zero
	file.open("Highscore.txt", std::ios::trunc);
	file << highScore;
	file.close();
}

int SavingSystem::LoadHighScore()
{
	if (!std::filesystem::exists("Highscore.txt")) { return 0; }

	//code adapted from Cplusplus.com (2023)
	std::fstream myfile("Highscore.txt", std::ios_base::in);
	int a = 0;
	myfile >> a;
	//function used to read once character at a time from an input source
	getchar();
	return a;
}

SavingSystem::~SavingSystem()
{
}
