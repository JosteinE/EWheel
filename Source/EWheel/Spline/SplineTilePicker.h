// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class MeshLibrary;
class UStaticMesh;

struct TileDetails
{
	int m_MeshCategory = 0;
	int m_MeshType = 0;
	int m_Rotation = 0; // Multiplied by 90 degrees
};
/**
 * 
 */
class EWHEEL_API SplineTilePicker
{
public:
	SplineTilePicker();
	~SplineTilePicker();
	
	TArray<UStaticMesh*> GetNewTiles(int numTiles);
	void SetNumRowsToLog(int num);
private:
	// Empties TileLog if it exceedes the set amount to store
	void CheckAndEmptyLog(int numTilesPerRow);


	// Get a list of indecies of dependant tiles belonging to the previous row
	void GetPreviousDependancies(TArray<bool>& indexLog, int numToCheck);
	
	// Checks if the tile behind depends on the new tile
	bool CheckDependancyPrevious(int currentIndex, int numTilesPerRow);

	// Checks if the left tile depends on the new tile
	bool CheckDependancyLeft(int currentIndex);

	// Checks if left tile was part of a ramp and if the next previous depends on a pit. If true, end the ramp
	bool CheckForTileCrash(int currentIndex, int numTilesPerRow);


	// Get the first tile in a row based on previous tile rows, dependantType = tile type from last row that depends on new tile 
	void GetAppropriateFirstTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow);

	// Get a tile based on previous tile rows, dependantType = tile type from last row that depends on new tile 
	void GetAppropriateTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow);

	// Get the last tile in a row based on previous tile rows, dependantType = tile type from last row that depends on new tile 
	void GetAppropriateLastTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow);

	// MeshType and Num 90 degree rotations
	TArray<TileDetails*> TileLog;
	// Number of rows to store in TileLog
	int NumRowsToLog = 5;

	MeshLibrary* MeshLib;
};
