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
		TileDetails* newTile = nullptr;

		// Generating from left to right, checking previous and left tile
			
		if (i == 0)
		{
			newTile = GetAppropriateFirstTile(numTiles);
		}
		else if (i < numTiles - 1)
		{
			newTile = GetAppropriateTile(numTiles);
		}
		else
		{
			newTile = GetAppropriateLastTile(numTiles);
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

void SplineTilePicker::GetPreviousDependancies(TArray<bool>& indexLog, int numToCheck)
{
	int loopStart = TileLog.Num() - 1 - numToCheck;
	for (int i = loopStart; i < TileLog.Num(); i++)
	{
		indexLog[i - loopStart] = CheckDependancyPrevious(i);
	}
}

bool SplineTilePicker::CheckDependancyPrevious(int indexLog)
{
	if (TileLog[indexLog]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
	{
		switch (TileLog[indexLog]->m_MeshType)
		{
		case PIT_4W:
			return true;
			break;
		case PIT_EX:
			if (TileLog[indexLog]->m_Rotation == 0)
				return true;
			break;
		case PIT_L:
			if (TileLog[indexLog]->m_Rotation == 0 || TileLog[indexLog]->m_Rotation == 3)
				return true;
			break;
		case PIT_T:
			if (TileLog[indexLog]->m_Rotation != 0)
				return true;
			break;
		default:
			break;
		}
	}
	return false;
}

bool SplineTilePicker::CheckDependancyLeft(int logIndex)
{
	if (TileLog[logIndex]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
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
	else if (TileLog[logIndex]->m_MeshCategory == MeshCategories::CATEGORY_RAMP)
	{
		if (TileLog[logIndex]->m_MeshType == MeshType::RAMP_L || TileLog[logIndex]->m_MeshType == MeshType::RAMP_M)
			return true;
	}
	return false;
}

bool SplineTilePicker::CheckForTileCrash(int logIndex, int numTilesPerRow)
{
	if (TileLog[logIndex]->m_MeshType == RAMP_L || TileLog[logIndex]->m_MeshType == RAMP_M)
		return CheckDependancyPrevious(logIndex - numTilesPerRow + 1);

	return false;
}

TileDetails* SplineTilePicker::GetAppropriateFirstTile(int numTilesPerRow)
{
	TileDetails* newTile = new TileDetails;

	if (!bPrevious)
	{
		// Generate random tile that doesnt depend on a left or previous tile
	}
	else // Means previous tile was a pit
	{
		// Generate tile that links with the previous pit tile and doesnt depend on a left tile
	}
	return newTile;
}

TileDetails* SplineTilePicker::GetAppropriateTile(int numTilesPerRow)
{
	TileDetails* newTile = new TileDetails;

	int currentIndex = TileLog.Num();

	if (CheckForTileCrash(currentIndex, numTilesPerRow))
	{
		newTile->m_MeshCategory = MeshCategories::CATEGORY_RAMP;
		newTile->m_MeshType = MeshType::RAMP_R;
		return newTile;
	}

	bool bLeft = CheckDependancyLeft(currentIndex - 1);
	bool bPrevious = CheckDependancyPrevious(currentIndex - numTilesPerRow);

	TArray<FIntVector> possibleTiles; // Category, Tile and Rotation

	if (bPrevious && bLeft)
	{
		// generate a tile that links with left and previous (We know these are two pits)
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_4W, 0});
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 1 });
	}
	else if (bPrevious)
	{
		// generate a tile that links with previous and NOT left (We know this is a pit)
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 1 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 3 });
	}
	else if (bLeft)
	{
		// generate a tile that links with left and NOT previous
		switch (TileLog[currentIndex - 1]->m_MeshCategory)
		{
		case MeshCategories::CATEGORY_PIT:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 1 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 1 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 2 });
			break;
		case MeshCategories::CATEGORY_RAMP:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_M, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_R, 0 });
			break;
		}
	}

	return newTile;
}

TileDetails* SplineTilePicker::GetAppropriateLastTile(int numTilesPerRow)
{
	TileDetails* newTile = new TileDetails;

	if (bPrevious || bLeft)
	{
		switch (dependantType)
		{
		case MeshCategories::CATEGORY_PIT:
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
		case MeshCategories::CATEGORY_RAMP:
			// We know this belongs to left. End Ramp
			break;
		}
	}
	return newTile;
}
