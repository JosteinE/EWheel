// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class MeshLibrary;
class UStaticMesh;
struct TileDetails;
/**
 * 
 */
class EWHEEL_API SplineTilePicker
{
public:
	SplineTilePicker();
	~SplineTilePicker();
	
	TArray<UStaticMesh*> GetNewTiles(int numTiles);

	int GetTileRotation(int index);
	TArray<int> GetRowRotation(int index, int numPerRow); // index should be first tile in row (left)
	TArray<int> GetLastRowRotation(int numPerRow); // index should be first tile in row (left)
	void SetNumRowsToLog(int num);
	void SetUseHighResModels(bool useHighRes);

	void LoadFromJson(TSharedPtr<FJsonObject> inJson);

	// Number of rows to store in TileLog. Need at least 2 to check the previous row
	int NumRowsToLog = 2;

	bool bAddEdges = false;
	bool bSpawnPits = true;
	bool bSpawnRamps = true;
	bool bSpawnHoles = true;
private:
	// Deletes pointers and empties the array
	void EmptyTileLog();

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

	// Get a tile that fits with the previous tile. bool = if the tile is dependant
	bool CheckNeedSpecificEdge(TileDetails* inDetails, bool bLeft);

	// Get a tile that fits with the previous tile. bool = if the tile is dependant
	bool GetAppropriateFirstTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow);

	// Get a tile that fits with the previous and left tile. bool = if the tile is dependant
	bool GetAppropriateTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow);

	// Get a tile that fits with the previous and left tile, and that doesnt rely on a right tile. bool = if the tile is dependant
	bool GetAppropriateLastTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow);

	// Returns a random index for the possibleTiles vector, with the index chance being based on the tiles weight
	int GetRandomIndexBasedOnWeight(TArray<FIntVector>& possibleTiles);

	// Adds side mesh to the mesh vector (left side = index 0, right side = last index)
	void AddEdgeMesh(TArray<UStaticMesh*>& tileMesh, int numTilesPerRow);

	// Returns a single random tile (used for single lane paths)
	UStaticMesh* GetNewTile();

	// Mesh category, type and Num 90 degree rotations
	TArray<TileDetails*> TileLog;
	MeshLibrary* MeshLib;
	TMap<int, int> WeightMap;

public:
	TArray<TileDetails*> GetEdgeLessTileLog();
	FORCEINLINE TArray<TileDetails*>* GetTileLog() { return &TileLog; };
	FORCEINLINE void SetSpawnPits(bool spawnPits) { bSpawnPits = spawnPits; };
	FORCEINLINE void SetSpawnRamps(bool spawnRamps) { bSpawnRamps = spawnRamps; };
	FORCEINLINE void SetSpawnHoles(bool spawnHoles) { bSpawnHoles = spawnHoles; };
};
