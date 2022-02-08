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

TArray<UStaticMesh*> SplineTilePicker::GetNewTiles(int numTiles)
{
	TArray<UStaticMesh*> TileMesh;

	// Remove stored tileDetails if we're about to surpass the limit
	CheckAndEmptyLog(numTiles);

	for (int i = 0; i < numTiles; i++)
	{
		// IntVector: Category, Tile and Rotation
		TArray<FIntVector> possibleTiles;

		// Generating from left to right, checking previous and left tile
			
		if (i == 0)
		{
			GetAppropriateFirstTile(possibleTiles, numTiles);
		}
		else if (i < numTiles - 1)
		{
			GetAppropriateTile(possibleTiles, numTiles);
		}
		else
		{
			GetAppropriateLastTile(possibleTiles, numTiles);
		}

		// Pick a random, possible tile
		int randomTileIndex = FMath::RandRange(0, possibleTiles.Num() - 1);

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

int SplineTilePicker::GetTileRotation(int index)
{
	return TileLog[index]->m_Rotation;
}

TArray<int> SplineTilePicker::GetRowRotation(int index, int numPerRow)
{
	TArray<int> tileRotations;
	for (int i = 0; i < numPerRow; i++)
	{
		tileRotations.Emplace(GetTileRotation(index));
	}
	return tileRotations;
}

TArray<int> SplineTilePicker::GetLastRowRotation(int index, int numPerRow)
{
	return GetRowRotation(TileLog.Num() - 1 - numPerRow, numPerRow);
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
		indexLog[i - loopStart] = CheckDependancyPrevious(i, numToCheck);
	}
}

bool SplineTilePicker::CheckDependancyPrevious(int currentIndex, int numTilesPerRow)
{
	int previousIndex = currentIndex - numTilesPerRow;
	if (previousIndex < 0) return false;

	if (TileLog[previousIndex]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
	{
		switch (TileLog[previousIndex]->m_MeshType)
		{
		case PIT_4W:
			return true;
			break;
		case PIT_EX:
			if (TileLog[previousIndex]->m_Rotation == 0)
				return true;
			break;
		case PIT_L:
			if (TileLog[previousIndex]->m_Rotation == 0 || TileLog[previousIndex]->m_Rotation == 3)
				return true;
			break;
		case PIT_T:
			if (TileLog[previousIndex]->m_Rotation != 0)
				return true;
			break;
		default:
			break;
		}
	}
	return false;
}

bool SplineTilePicker::CheckDependancyLeft(int currentIndex)
{
	int leftIndex = currentIndex - 1;
	if (TileLog[leftIndex]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
	{
		switch (TileLog[leftIndex]->m_MeshType)
		{
		case PIT_4W:
			return true;
		case PIT_EX:
			if (TileLog[leftIndex]->m_Rotation == 1)
				return true;
			break;
		case PIT_L:
			if (TileLog[leftIndex]->m_Rotation == 0 || TileLog[leftIndex]->m_Rotation == 1)
				return true;
			break;
		case PIT_T:
			if (TileLog[leftIndex]->m_Rotation != 1)
				return true;
			break;
		default:
			break;
		}
	}
	else if (TileLog[leftIndex]->m_MeshCategory == MeshCategories::CATEGORY_RAMP)
	{
		if (TileLog[leftIndex]->m_MeshType == MeshType::RAMP_L || TileLog[leftIndex]->m_MeshType == MeshType::RAMP_M)
			return true;
	}
	return false;
}

bool SplineTilePicker::CheckForTileCrash(int currentIndex, int numTilesPerRow)
{
	if (currentIndex > 0 && (TileLog[currentIndex - 1]->m_MeshType == RAMP_L || TileLog[currentIndex - 1]->m_MeshType == RAMP_M))
		return CheckDependancyPrevious(currentIndex, numTilesPerRow);

	return false;
}

void SplineTilePicker::GetAppropriateFirstTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	if (CheckForTileCrash(currentIndex, numTilesPerRow))
	{
		// NO ramps
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
	}
	else if (CheckDependancyPrevious(currentIndex, numTilesPerRow)) // Means previous tile was a pit
	{
		// Generate tile that links with the previous pit tile and NOT left tile
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 1 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 3 });
	}
	else
	{
		// Generate random tile that DOESNT link to left or previous tile
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_L, 0 });
	}
}

void SplineTilePicker::GetAppropriateTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	if (CheckForTileCrash(currentIndex, numTilesPerRow))
	{
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_R, 0 });
		return;
	}

	bool bLeft = CheckDependancyLeft(currentIndex);
	bool bPrevious = CheckDependancyPrevious(currentIndex, numTilesPerRow);

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
	else
	{
		// generate a tile that doesnt depend on previous or left
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_L, 0 });
	}
}

void SplineTilePicker::GetAppropriateLastTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	bool bLeft = CheckDependancyLeft(currentIndex);
	bool bPrevious = CheckDependancyPrevious(currentIndex, numTilesPerRow);


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
	else
	{
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
	}
}
