// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct HighscoreSlot
{
	int mRank;
	FString mName;
	int mScore;
	FString mTime;
	int mDistance;
};
/**
 * 
 */
class EWHEEL_API HighscoreWriter
{
public:
	HighscoreWriter();
	~HighscoreWriter();
	void AddToHighscore(HighscoreSlot& inPlayer);
};
