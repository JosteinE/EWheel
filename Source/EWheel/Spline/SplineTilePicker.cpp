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
	TArray<int> PreviousDependantTiles; // 0 -> numTiles = previous row tile indices (left to right)
	
	if (TileLog.Num() >= numTiles)
	{
		// Populate the array with false bools and assign them true in GetPreviousDependancies
		PreviousDependantTiles.Init(-1, numTiles);
		GetPreviousDependancies(PreviousDependantTiles, numTiles);
	}

	for (int i = 0; i < numTiles; i++)
	{
		TileDetails* newTile = new TileDetails;

		if (TileLog.Num() >= numTiles)
		{
			// Generate a new row of tiles based on history
			// Generating from left to right, checking previous and left tile
			
			// Check last created tile if not first 
			if (i > 0)
			{
				newTile = GetAppropriateTile(PreviousDependantTiles[i], CheckDependancyLeft(TileLog.Num() - 1), 1); // CHANGE 1 TO TYPE!
			}
			else
			{
				newTile = GetAppropriateFirstTile(PreviousDependantTiles[i], 1); // CHANGE 1 TO TYPE!
			}

		}
		else
		{
			// Generate the first row of tiles to go on the spline

			// Check last created tile if not first 
			if (i > 0)
			{
				newTile = GetAppropriateTile(false, CheckDependancyLeft(TileLog.Num() - 1), 1); // CHANGE 1 TO TYPE!
			}
			else
			{
				newTile = GetAppropriateTile(false, false, 1); // CHANGE 1 TO TYPE!
			}
		}

		// Log to pair
		TileLog.Emplace(newTile);

		//ROTATION IS NOT APPLIED TO MESH HERE
		UStaticMesh* newMesh = MeshLib->GetMesh(newTile->m_MeshType);
		TileMesh.Emplace(newMesh);
	}
	return TileMesh;
}

void SplineTilePicker::SetNumToLog(int num)
{
	NumToLog = num;
}

void SplineTilePicker::GetPreviousDependancies(TArray<int>& indexLog, int numToCheck)
{
	int loopStart = TileLog.Num() - 1 - numToCheck;
	for (int i = loopStart; i < TileLog.Num(); i++)
	{
		if (TileLog[i]->m_MeshCategory == MeshCategories::TYPE_PIT)
		{
			switch (TileLog[i]->m_MeshType)
			{
			case PIT_4W:
				indexLog[i - loopStart] = i;
				break;
			case PIT_EX:
				if(TileLog[i]->m_Rotation == 0)
					indexLog[i - loopStart] = i;
				break;
			case PIT_L:
				if(TileLog[i]->m_Rotation == 0 || TileLog[i]->m_Rotation == 3)
					indexLog[i - loopStart] = i;
				break;
			case PIT_T:
				if(TileLog[i]->m_Rotation != 0)
					indexLog[i - loopStart] = i;
				break;
			default:
				break;
			}
		}
	}
}

bool SplineTilePicker::CheckDependancyLeft(int logIndex)
{
	if (TileLog[logIndex]->m_MeshCategory == MeshCategories::TYPE_PIT)
	{
		switch (TileLog[logIndex]->m_MeshType)
		{
		case PIT_4W:
			return true;
		case PIT_EX:
			if (TileLog[logIndex]->m_Rotation == 1)
				return true;
			break;
		case PIT_L:
			if (TileLog[logIndex]->m_Rotation == 0 || TileLog[logIndex]->m_Rotation == 1)
				return true;
			break;
		case PIT_T:
			if (TileLog[logIndex]->m_Rotation != 1)
				return true;
			break;
		default:
			break;
		}
	}
	else if (TileLog[logIndex]->m_MeshCategory == MeshCategories::TYPE_RAMP)
	{
		if (TileLog[logIndex]->m_MeshType == MeshType::RAMP_L || TileLog[logIndex]->m_MeshType == MeshType::RAMP_M)
			return true;
	}
	return false;
}

TileDetails* SplineTilePicker::GetAppropriateFirstTile(int previousTile)
{
	TPair<int, int>* newTile = new TPair<int, int>;

	if (!bPrevious)
	{
		// Generate random tile that doesnt depend on a left or previous tile
	}
	else if(dependantType == MeshCategories::TYPE_PIT)
	{
		// Generate tile that links with the previous pit tile and doesnt depend on a left tile
	}
	return newTile;
}

TileDetails* SplineTilePicker::GetAppropriateTile(int previousTile, bool bLeft)
{
	TPair<int, int>* newTile = new TPair<int, int>;

	if (bPrevious || bLeft)
	{
		switch (dependantType)
		{
		case MeshCategories::TYPE_PIT:
			if (bPrevious && bLeft)
			{
				// generate a tile that links with left and previous
			}
			else if (bPrevious)
			{
				// generate a tile that links with previous and NOT left
			}
			else
			{
				// generate a tile that links with left and NOT previous
			}
			break;
		case MeshCategories::TYPE_RAMP:
			// We know this belongs to left. Either end ramp or extend
			break;

		default:
			break;
		}
	}
	return newTile;
}

TileDetails* SplineTilePicker::GetAppropriateLastTile(int previousTile, bool bLeft)
{
	TPair<int, int>* newTile = new TPair<int, int>;

	if (bPrevious || bLeft)
	{
		switch (dependantType)
		{
		case MeshCategories::TYPE_PIT:
			if (bPrevious && bLeft)
			{
				// generate a tile that links with left and previous, DO NOT DEPEND ON RIGHT
			}
			else if (bPrevious)
			{
				// generate a tile that links with previous and NOT left, DO NOT DEPEND ON RIGHT
			}
			else
			{
				// generate a tile that links with left and NOT previous, DO NOT DEPEND ON RIGHT
			}
			break;
		case MeshCategories::TYPE_RAMP:
			// We know this belongs to left. End Ramp
			break;
		}
	}
	return newTile;
}
