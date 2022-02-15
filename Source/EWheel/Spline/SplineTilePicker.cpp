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

	EmptyTileLog();
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

		bool bTileIsDependant = false;
		// Generating from left to right, checking previous and left tile
		if (i == 0)
		{
			bTileIsDependant = GetAppropriateFirstTile(possibleTiles, numTiles);
		}
		else if (i < numTiles - 1)
		{
			bTileIsDependant = GetAppropriateTile(possibleTiles, numTiles);
		}
		else
		{
			bTileIsDependant = GetAppropriateLastTile(possibleTiles, numTiles);
		}

		// Pick a random, possible tile
		int randomTileIndex = 0;
		if (!bTileIsDependant && FMath::RandRange(0, 99) < FlatBoyChance)
		{
			possibleTiles.Empty();
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_DEFAULT, MeshType::DEFAULT, 0 });
		}
		else
		{
			randomTileIndex = GetRandomIndexBasedOnWeight(possibleTiles);
		}

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
		tileRotations.Emplace(GetTileRotation(index + i));
	}
	return tileRotations;
}

TArray<int> SplineTilePicker::GetLastRowRotation(int numPerRow)
{
	return GetRowRotation(TileLog.Num() - numPerRow, numPerRow);
}

void SplineTilePicker::SetNumRowsToLog(int num)
{
	NumRowsToLog = num;
}

void SplineTilePicker::EmptyTileLog()
{
	for (int i = 0; i < TileLog.Num(); i++)
	{
		delete TileLog[i];
		TileLog[i] = nullptr;
	}
	TileLog.Empty();
}

void SplineTilePicker::CheckAndEmptyLog(int numTilesPerRow)
{
	if (TileLog.Num() + numTilesPerRow > NumRowsToLog * numTilesPerRow)
		TileLog.RemoveAt(0, numTilesPerRow, true);
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

	switch (TileLog[previousIndex]->m_MeshType)
	{
	// Pits
	case PIT_4W:
		return true;
		break;
	case PIT_END_SN:
		if (TileLog[previousIndex]->m_Rotation == 2)
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
	return false;
}

bool SplineTilePicker::CheckDependancyLeft(int currentIndex)
{
	int leftIndex = currentIndex - 1;
	if (leftIndex < 0) return false;

	switch (TileLog[leftIndex]->m_MeshType)
	{
	// Pits
	case PIT_4W:
		return true;
	case PIT_END_SN:
		if (TileLog[leftIndex]->m_Rotation == 3)
			return true;
		break;
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

	//Ramps
	case RAMP_L:
		return true;
	case RAMP_M:
		return true;

	default:
		break;
	}
	return false;
}

bool SplineTilePicker::CheckForTileCrash(int currentIndex, int numTilesPerRow)
{
	//bool bLeft = (currentIndex > 0 && (currentIndex - 1) % numTilesPerRow == 0) && TileLog[currentIndex - 1]->m_MeshType == RAMP_L || TileLog[currentIndex - 1]->m_MeshType == RAMP_M;
	//bool bNextPrevious = CheckDependancyPrevious(currentIndex + 1, numTilesPerRow);

	return CheckDependancyPrevious(currentIndex + 1, numTilesPerRow);
}

bool SplineTilePicker::GetAppropriateFirstTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	if (CheckDependancyPrevious(currentIndex, numTilesPerRow)) // Means previous tile was a pit
	{
		// Generate tile that links with the previous pit tile and NOT left tile
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 1 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 3 });

		return true;
	}
	else if (numTilesPerRow > 1 && CheckDependancyPrevious(currentIndex + 1, numTilesPerRow))
	{
		// NO CONNECTED RAMPS
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		
		return true;
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

		return false;
	}
}

bool SplineTilePicker::GetAppropriateTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	bool bLeft = CheckDependancyLeft(currentIndex);
	bool bPrevious = CheckDependancyPrevious(currentIndex, numTilesPerRow);

	if (bPrevious && bLeft)
	{
		// generate a tile that links with left and previous (We know these are two pits)
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_4W, 0});
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 1 });

		return true;
	}
	else if (bPrevious)
	{
		// generate a tile that links with previous and NOT left (We know this is a pit)
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 1 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 3 });

		return true;
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
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_R, 0 });
			if (!CheckDependancyPrevious(currentIndex + 1, numTilesPerRow))
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_M, 0 });
			break;
		}

		return true;
	}
	else
	{
		// generate a tile that doesnt depend on previous or left
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		
		if(!CheckDependancyPrevious(currentIndex + 1, numTilesPerRow))
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_L, 0 });

		return false;
	}
}

bool SplineTilePicker::GetAppropriateLastTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	bool bLeft = CheckDependancyLeft(currentIndex);
	bool bPrevious = CheckDependancyPrevious(currentIndex, numTilesPerRow);


	if (bPrevious && bLeft)
	{
		// generate a tile that links with left and previous (We know these are two pits), DO NOT DEPEND ON RIGHT
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 1 });

		return true;
	}
	else if (bPrevious)
	{
		// generate a tile that links with previous and NOT left (We know this is a pit), DO NOT DEPEND ON RIGHT
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });

		return true;
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

		return true;
	}
	else
	{
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });

		return false;
	}
}

int SplineTilePicker::GetRandomIndexBasedOnWeight(TArray<FIntVector>& possibleTiles)
{
	TArray<int> weightMap;

	// Assign every option a weight. The higher the weight, the better the chance to get picked
	for (int i = 0; i < possibleTiles.Num(); i++)
	{
		if (possibleTiles[i].X == MeshCategories::CATEGORY_PIT)
		{
			switch (possibleTiles[i].Y)
			{
			case MeshType::PIT:
				weightMap.Emplace(4);
				break;
			case MeshType::PIT_4W:
				weightMap.Emplace(1);
				break;
			case MeshType::PIT_END_SN:
				weightMap.Emplace(10);
				break;
			case MeshType::PIT_EX:
				weightMap.Emplace(3);
				break;
			case MeshType::PIT_L:
				weightMap.Emplace(3);
				break;
			case MeshType::PIT_T:
				weightMap.Emplace(2);
				break;
			default:
				break;
			}
		}
		else if (possibleTiles[i].X == MeshCategories::CATEGORY_RAMP)
		{
			switch (possibleTiles[i].Y)
			{
			case MeshType::RAMP:
				weightMap.Emplace(3);
				break;
			case MeshType::RAMP_L:
				weightMap.Emplace(5);
				break;
			case MeshType::RAMP_M:
				weightMap.Emplace(2);
				break;
			case MeshType::RAMP_R:
				weightMap.Emplace(5);
				break;
			default:
				break;
			}
		}
		// NEW & UNTESTED
		else if (possibleTiles[i].X == MeshCategories::CATEGORY_HOLE)
		{
			switch (possibleTiles[i].Y)
			{
			case MeshType::HOLE:
				weightMap.Emplace(5);
				break;
			case MeshType::HOLE_END_SN:
				weightMap.Emplace(5);
				break;
			case MeshType::HOLE_EX:
				weightMap.Emplace(5);
				break;
			case MeshType::HOLE_L:
				weightMap.Emplace(5);
				break;
			case MeshType::HOLE_T:
				weightMap.Emplace(5);
				break;
			default:
				break;
			}
		}
	}

	int sumWeight = 0;

	// Calculate the sum of the weights
	for (int i = 0; i < weightMap.Num(); i++)
	{
		sumWeight += weightMap[i];
	}

	int sumRandom = FMath::RandRange(0, sumWeight);

	// Iterate through the map, subtracting the current item's weight from the random number
	// untill one is picked.
	for (int i = 0; i < weightMap.Num(); i++)
	{
		if (sumRandom < weightMap[i])
			return i;
		sumRandom -= weightMap[i];
	}

	return 0;
}