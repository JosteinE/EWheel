// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/SplineTilePicker.h"
#include "EWheel/Spline/MeshLibrary.h"
#include "Engine/StaticMesh.h"

SplineTilePicker::SplineTilePicker()
{
	MeshLib = new MeshLibrary;
}

SplineTilePicker::~SplineTilePicker()
{
	delete MeshLib;
	MeshLib = nullptr;

	for (int i = 0; i < TileLog.Num(); i++)
	{
		delete TileLog[i];
		TileLog[i] = nullptr;
	}
	TileLog.Empty();
}

TArray<UStaticMesh*> SplineTilePicker::GetTiles(int numTiles)
{
	TArray<UStaticMesh*> TileMesh;

	for (int i = 0; i < numTiles; i++)
	{
		TPair<int, int>* newTile;
		if (TileLog.Num() > numTiles)
		{
			// Generate a new row of tiles based on history
			// Generating from left to right, checking previous and left tile
			
			// Check last created tile if not first 
			if (i > 0)
			{
				newTile = GetAppropriateTile(CheckDependancyPrevious(TileLog.Num() - 1 - numTiles), CheckDependancyLeft(TileLog.Num() - 2), 1); // CHANGE 1 TO TYPE!
			}
			else
			{
				newTile = GetAppropriateTile(CheckDependancyPrevious(TileLog.Num() - 1 - numTiles), false, 1); // CHANGE 1 TO TYPE!
			}

		}
		else
		{
			// Generate the first row of tiles to go on the spline

			// Check last created tile if not first 
			if (i > 0)
			{
				newTile = GetAppropriateTile(false, CheckDependancyLeft(TileLog.Num() - 2), 1); // CHANGE 1 TO TYPE!
			}
			else
			{
				newTile = GetAppropriateTile(false, false, 1); // CHANGE 1 TO TYPE!
			}
		}

		// Log to pair
		TileLog.Emplace(newTile);

		//ROTATION IS NOT APPLIED TO MESH HERE
		UStaticMesh* newMesh = MeshLib->GetMesh(newTile->Key);
		TileMesh.Emplace(newMesh);
	}
	return TileMesh;
}

void SplineTilePicker::SetNumToLog(int num)
{
	NumToLog = num;
}

bool SplineTilePicker::CheckDependancyPrevious(int logIndex)
{
	return false;
}

bool SplineTilePicker::CheckDependancyLeft(int logIndex)
{
	return false;
}

TPair<int, int>* SplineTilePicker::GetAppropriateTile(bool bPrevious, bool bLeft, int tileType)
{
	TPair<int, int>* newTile = new TPair<int, int>;

	if(tileType = )

	return newTile;
}
