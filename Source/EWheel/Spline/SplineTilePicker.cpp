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

	// Remove stored tileDetails if we're about to surpass the limit
	CheckAndEmptyLog(numTiles);

	for (int i = 0; i < numTiles; i++)
	{
		TArray<FIntVector> possibleTiles;

		// Generating from left to right, checking previous and left tile
			
		if (i == 0)
		{
			possibleTiles = GetAppropriateFirstTile(numTiles);
		}
		else if (i < numTiles - 1)
		{
			possibleTiles = GetAppropriateTile(numTiles);
		}
		else
		{
			possibleTiles = GetAppropriateLastTile(numTiles);
		}

		// Pick a random, possible tile
		int randomTileIndex = FMath::RandRange(0, possibleTiles.Num());

		// Import data from random possible tile to a new tile
		TileDetails* newTile = new TileDetails;
		newTile->m_MeshCategory = possibleTiles[randomTileIndex].X;
		newTile->m_MeshType = possibleTiles[randomTileIndex].Y;
		newTile->m_Rotation = possibleTiles[randomTileIndex].Z;

		// Log to pair
		TileLog.Emplace(newTile);

		//ROTATION IS NOT APPLIED TO MESH HERE
		UStaticMesh* newMesh = MeshLib->GetMesh(newTile->m_MeshType);
		TileMesh.Emplace(newMesh);
	}
	return TileMesh;
}

void SplineTilePicker::SetNumRowsToLog(int num)
{
	NumRowsToLog = num;
}

void SplineTilePicker::CheckAndEmptyLog(int numTilesPerRow)
{
	if (TileLog.Num() + numTilesPerRow > NumRowsToLog * numTilesPerRow)
	{
		TileLog.RemoveAt(0, numTilesPerRow, true);
	}
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

bool SplineTilePicker::CheckForTileCrash(int currentIndex, int numTilesPerRow)
{
	if (TileLog[currentIndex - 1]->m_MeshType == RAMP_L || TileLog[currentIndex - 1]->m_MeshType == RAMP_M)
		return CheckDependancyPrevious(currentIndex - numTilesPerRow + 1);

	return false;
}

TArray<FIntVector> SplineTilePicker::GetAppropriateFirstTile(int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	TArray<FIntVector> possibleTiles; // Category, Tile and Rotation

	if (TileLog.Num() < numTilesPerRow)
	{
		// Generate random tile that DOESNT link to left or previous tile
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_L, 0 });
	}
	else if (CheckForTileCrash(currentIndex, numTilesPerRow))
	{
		// NO ramps
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
	}
	if (CheckDependancyPrevious(currentIndex)) // Means previous tile was a pit
	{
		// Generate tile that links with the previous pit tile and NOT left tile
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 1 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 3 });
	}
	return possibleTiles;
}

TArray<FIntVector> SplineTilePicker::GetAppropriateTile(int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	TArray<FIntVector> possibleTiles; // Category, Tile and Rotation

	if (CheckForTileCrash(currentIndex, numTilesPerRow))
	{
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_R, 0 });
		return possibleTiles;
	}

	bool bLeft = CheckDependancyLeft(currentIndex - 1);
	bool bPrevious = CheckDependancyPrevious(currentIndex - numTilesPerRow);

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

	return possibleTiles;
}

TArray<FIntVector> SplineTilePicker::GetAppropriateLastTile(int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	TArray<FIntVector> possibleTiles; // Category, Tile and Rotation

	bool bLeft = CheckDependancyLeft(currentIndex - 1);
	bool bPrevious = CheckDependancyPrevious(currentIndex - numTilesPerRow);


	if (bPrevious && bLeft)
	{
		// generate a tile that links with left and previous (We know these are two pits), DO NOT DEPEND ON RIGHT
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 1 });
	}
	else if (bPrevious)
	{
		// generate a tile that links with previous and NOT left (We know this is a pit), DO NOT DEPEND ON RIGHT
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
	}
	else if (bLeft)
	{
		// generate a tile that links with left and NOT previous, DO NOT DEPEND ON RIGHT
		switch (TileLog[currentIndex - 1]->m_MeshCategory)
		{
		case MeshCategories::CATEGORY_PIT:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 1 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 3 });
			break;
		case MeshCategories::CATEGORY_RAMP:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_R, 0 });
			break;
		}
	}
	return possibleTiles;
}
