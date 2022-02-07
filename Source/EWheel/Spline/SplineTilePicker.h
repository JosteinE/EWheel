// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class MeshLibrary;
class UStaticMesh;
/**
 * 
 */
class EWHEEL_API SplineTilePicker
{
public:
	SplineTilePicker();
	~SplineTilePicker();
	
	TArray<UStaticMesh*> GetTiles(int numTiles);
	void SetNumToLog(int num);
private:
	bool CheckDependancyPrevious(int logIndex);
	bool CheckDependancyLeft(int logIndex);
	TPair<int, int> GetAppropriateTile(bool bPrevious, bool bLeft, int tileType = 0);

	// MeshType and Num 90 degree rotations
	TArray<TPair<int, int>> TileLog;
	// Number of rows to store in TileLog
	int NumToLog = 5;

	MeshLibrary* MeshLib;
};
