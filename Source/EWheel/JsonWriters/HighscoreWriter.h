// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EWheel/JsonWriters/JsonWriterBase.h"

struct HighscoreSlot
{
	FString mName;
	int mScore;
	FString mTime;
	int mDistance;
};
/**
 * 
 */
class EWHEEL_API HighscoreWriter : private UJsonWriterBase
{
public:
	HighscoreWriter();
	~HighscoreWriter();
	void AddToHighscore(HighscoreSlot& inPlayer, FString& inMode);
	void ImportFromHighscoreData(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, HighscoreSlot& inPlayer);
};
