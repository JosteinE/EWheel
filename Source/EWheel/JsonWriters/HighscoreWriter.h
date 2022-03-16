// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"

struct HighscoreSlot
{
	FString mName = "";
	int mScore = 0;
	FString mTime = "00:00:00";
	float mDistance = 0.f;
};

/**
 * 
 */
class EWHEEL_API HighscoreWriter
{
public:
	HighscoreWriter();
	~HighscoreWriter();

	void AddToHighscore(HighscoreSlot& inPlayer, FString& inMode);
private:
	void ImportFromHighscoreData(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, HighscoreSlot& inPlayer);
};
