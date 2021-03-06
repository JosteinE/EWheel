// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerDetails.generated.h"

USTRUCT(BlueprintType)
struct FPlayerDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString mName = "";

	UPROPERTY(BlueprintReadWrite)
	int mPointsCollected = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bFender = false;

	UPROPERTY(BlueprintReadWrite)
	int mSkinSelected = 0;

	UPROPERTY(BlueprintReadWrite)
	TArray<bool> mSkinsUnlocked;

	// Rail, Bumper, Footpad, Fender
	UPROPERTY(BlueprintReadWrite)
	TArray<FVector> mBoardColours;

	FPlayerDetails()
	{
		mSkinsUnlocked.Append({ true, false, false, false });
		mBoardColours.Init(FVector(0), 4);
	};
};