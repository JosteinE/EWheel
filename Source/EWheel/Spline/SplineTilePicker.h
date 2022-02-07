// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class MeshLibrary;
class UStaticMesh;

struct TileDetails
{
	int m_MeshType;
	int m_MeshCategory;
	int m_Rotation; // Multiplied by 90 degrees
};
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
	void GetPreviousDependancies(TArray<int>& indexLog, int numToCheck);
	bool CheckDependancyLeft(int logIndex);


	// Get the first tile in a row based on previous tile rows, dependantType = tile type from last row that depends on new tile 
	TileDetails* GetAppropriateFirstTile(int previousTile, int currentIndex = 0);

	// Get a tile based on previous tile rows, dependantType = tile type from last row that depends on new tile 
	TileDetails* GetAppropriateTile(int previousTile, bool bLeft, int currentIndex = 0);

	// Get the last tile in a row based on previous tile rows, dependantType = tile type from last row that depends on new tile 
	TileDetails* GetAppropriateLastTile(int previousTile, bool bLeft, int currentIndex = 0);

	// MeshType and Num 90 degree rotations
	TArray<TileDetails*> TileLog;
	// Number of rows to store in TileLog
	int NumToLog = 5;

	MeshLibrary* MeshLib;
};
