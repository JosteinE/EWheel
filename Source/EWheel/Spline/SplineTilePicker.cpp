// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/SplineTilePicker.h"
#include "EWheel/Spline/MeshLibrary.h"
#include "EWheel/Spline/TileDetails.h"
#include "EWheel/Spline/MeshCategoriesAndTypes.h"

SplineTilePicker::SplineTilePicker()
{
	MeshLib = new MeshLibrary;

	WeightMap.Add(MeshType::DEFAULT);
	WeightMap.Add(MeshType::PIT);
	WeightMap.Add(MeshType::PIT_4W);
	WeightMap.Add(MeshType::PIT_END_SN);
	WeightMap.Add(MeshType::PIT_EX);
	WeightMap.Add(MeshType::PIT_L);
	WeightMap.Add(MeshType::PIT_T);
	WeightMap.Add(MeshType::RAMP);
	WeightMap.Add(MeshType::RAMP_L);
	WeightMap.Add(MeshType::RAMP_M);
	WeightMap.Add(MeshType::RAMP_R);
	WeightMap.Add(MeshType::HOLE);
	WeightMap.Add(MeshType::HOLE_4W);
	WeightMap.Add(MeshType::HOLE_END_SN);
	WeightMap.Add(MeshType::HOLE_EX);
	WeightMap.Add(MeshType::HOLE_L);
	WeightMap.Add(MeshType::HOLE_T);
}

SplineTilePicker::~SplineTilePicker()
{
	//delete MeshLib;
	//MeshLib = nullptr;

	EmptyTileLog();
}

TArray<UStaticMesh*> SplineTilePicker::GetNewTiles(int numTiles)
{
	TArray<UStaticMesh*> TileMesh;

	// Remove stored tileDetails if we're about to surpass the limit
	CheckAndEmptyLog(numTiles);

	if(numTiles == 1)
	{
		TileMesh.Emplace(GetNewTile());
		if(bAddEdges)
			AddEdgeMesh(TileMesh, numTiles);

		return TileMesh;
	}

	for (int i = 0; i < numTiles; i++)
	{
		// IntVector: Category, Tile and Rotation
		TArray<FIntVector> possibleTiles;

		bool bTileIsDependant = false;
		// Generating from left to right, checking previous and left tile
		if (i == 0)
			bTileIsDependant = GetAppropriateFirstTile(possibleTiles, numTiles);
		else if (i < numTiles - 1)
			bTileIsDependant = GetAppropriateTile(possibleTiles, numTiles);
		else
			bTileIsDependant = GetAppropriateLastTile(possibleTiles, numTiles);

		// Pick a random, possible tile
		int randomTileIndex = 0;
		if (!bTileIsDependant && FMath::RandRange(0, 99) < WeightMap[MeshType::DEFAULT])
		{
			possibleTiles.Empty();
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_DEFAULT, MeshType::DEFAULT, 0 });
		}
		else
		{
			randomTileIndex = GetRandomIndexBasedOnWeight(possibleTiles);

			if (randomTileIndex < 0)
			{
				possibleTiles.Empty();
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_DEFAULT, MeshType::DEFAULT, 0 });
				randomTileIndex = 0;
			}
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

	if (!bAddEdges) return TileMesh;

	AddEdgeMesh(TileMesh, numTiles);

	return TileMesh;
}

UStaticMesh* SplineTilePicker::GetNewTile()
{
	// Remove stored tileDetails if we're about to surpass the limit
	CheckAndEmptyLog(1);
	TArray<FIntVector> possibleTiles;
	int currentIndex = TileLog.Num();
	
	bool bTileIsDependant = false;
	if (CheckDependancyPrevious(currentIndex, 1)) // We know these are pits
	{
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
		bTileIsDependant = true;
	}
	else
	{
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_DEFAULT, MeshType::DEFAULT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE, 0 });
	}

	// Pick a random, possible tile
	int randomTileIndex = 0;
	if (!bTileIsDependant && FMath::RandRange(0, 99) < WeightMap[MeshType::DEFAULT])
	{
		possibleTiles.Empty();
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_DEFAULT, MeshType::DEFAULT, 0 });
	}
	else
	{
		randomTileIndex = GetRandomIndexBasedOnWeight(possibleTiles);

		if (randomTileIndex < 0)
		{
			possibleTiles.Empty();
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_DEFAULT, MeshType::DEFAULT, 0 });
			randomTileIndex = 0;
		}
	}

	// Import data from random possible tile to a new tile
	TileDetails* newTile = new TileDetails;
	newTile->m_MeshCategory = possibleTiles[randomTileIndex].X;
	newTile->m_MeshType = possibleTiles[randomTileIndex].Y;
	newTile->m_Rotation = possibleTiles[randomTileIndex].Z;

	// Log to pair
	TileLog.Emplace(newTile);

	return MeshLib->GetMesh(newTile->m_MeshType);
}

TArray<TileDetails*> SplineTilePicker::GetEdgeLessTileLog()
{
	TArray<TileDetails*> tLog = TileLog;
	tLog.RemoveAt(0, 1, false);
	tLog.RemoveAt(tLog.Num() * 0.5f - 1, 1, false);
	tLog.RemoveAt(tLog.Num() * 0.5f, 1, false);
	tLog.Pop(false);
	tLog.Shrink();

	return tLog;
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

void SplineTilePicker::SetUseHighResModels(bool useHighRes)
{
	MeshLib->SetUseHighResModels(useHighRes);
}

void SplineTilePicker::LoadFromJson(TSharedPtr<FJsonObject> inJson)
{
	WeightMap[MeshType::DEFAULT] = inJson->GetIntegerField("Default_Flat");
	WeightMap[MeshType::PIT] = inJson->GetIntegerField("Pit_Single");
	WeightMap[MeshType::PIT_END_SN] = inJson->GetIntegerField("Pit_End");
	WeightMap[MeshType::PIT_EX] = inJson->GetIntegerField("Pit_Ex");
	WeightMap[MeshType::PIT_L] = inJson->GetIntegerField("Pit_L");
	WeightMap[MeshType::PIT_T] = inJson->GetIntegerField("Pit_T");
	WeightMap[MeshType::PIT_4W] = inJson->GetIntegerField("Pit_4w");
	WeightMap[MeshType::RAMP] = inJson->GetIntegerField("Ramp_Single");
	WeightMap[MeshType::RAMP_L] = inJson->GetIntegerField("Ramp_L");
	WeightMap[MeshType::RAMP_M] = inJson->GetIntegerField("Ramp_M");
	WeightMap[MeshType::RAMP_R] = inJson->GetIntegerField("Ramp_R");
	WeightMap[MeshType::HOLE] = inJson->GetIntegerField("Hole_Single");
	WeightMap[MeshType::HOLE_4W] = inJson->GetIntegerField("Hole_4w");
	WeightMap[MeshType::HOLE_END_SN] = inJson->GetIntegerField("Hole_End");
	WeightMap[MeshType::HOLE_EX] = inJson->GetIntegerField("Hole_Ex");
	WeightMap[MeshType::HOLE_L] = inJson->GetIntegerField("Hole_L");
	WeightMap[MeshType::HOLE_T] = inJson->GetIntegerField("Hole_T");
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

	if (TileLog[previousIndex]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
	{
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
	}
	else if (TileLog[previousIndex]->m_MeshCategory == MeshCategories::CATEGORY_HOLE)
	{
		switch (TileLog[previousIndex]->m_MeshType)
		{
		case HOLE_4W:
			return true;
			break;
		case HOLE_END_SN:
			if (TileLog[previousIndex]->m_Rotation == 2)
				return true;
			break;
		case MeshType::HOLE_EX:
			if (TileLog[previousIndex]->m_Rotation == 0)
				return true;
			break;
		case MeshType::HOLE_L:
			if (TileLog[previousIndex]->m_Rotation == 0 || TileLog[previousIndex]->m_Rotation == 3)
				return true;
			break;
		case MeshType::HOLE_T:
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
	if (leftIndex < 0) return false;

	if (TileLog[leftIndex]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
	{
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

		default:
			break;
		}
	}
	if (TileLog[leftIndex]->m_MeshCategory == MeshCategories::CATEGORY_RAMP)
	{
		switch (TileLog[leftIndex]->m_MeshType)
		{
			//Ramps
		case RAMP_L:
			return true;
		case RAMP_M:
			return true;

		default:
			break;
		}
	}
	else if (TileLog[leftIndex]->m_MeshCategory == MeshCategories::CATEGORY_HOLE)
	{
		switch (TileLog[leftIndex]->m_MeshType)
		{
		case HOLE_4W:
			return true;
			break;
		case HOLE_END_SN:
			if (TileLog[leftIndex]->m_Rotation == 3)
				return true;
			break;
		case HOLE_EX:
			if (TileLog[leftIndex]->m_Rotation == 1)
				return true;
			break;
		case HOLE_L:
			if (TileLog[leftIndex]->m_Rotation == 0 || TileLog[leftIndex]->m_Rotation == 1)
				return true;
			break;
		case HOLE_T:
			if (TileLog[leftIndex]->m_Rotation != 1)
				return true;
			break;

		default:
			break;
		}
	}
	return false;
}

bool SplineTilePicker::CheckForTileCrash(int currentIndex, int numTilesPerRow)
{
	//bool bLeft = (currentIndex > 0 && (currentIndex - 1) % numTilesPerRow == 0) && TileLog[currentIndex - 1]->m_MeshType == RAMP_L || TileLog[currentIndex - 1]->m_MeshType == RAMP_M;
	//bool bNextPrevious = CheckDependancyPrevious(currentIndex + 1, numTilesPerRow);

	return CheckDependancyPrevious(currentIndex + 1, numTilesPerRow);
}

bool SplineTilePicker::CheckNeedSpecificEdge(TileDetails* inDetails, bool bLeft)
{
	if (inDetails->m_MeshCategory != MeshCategories::CATEGORY_HOLE) return false;

	switch (inDetails->m_MeshType)
	{
	case MeshType::HOLE_4W:
		return true;
	case MeshType::HOLE_END_SN:
		if ((bLeft && inDetails->m_Rotation == 1) || (!bLeft && inDetails->m_Rotation == 3))
			return true;
		break;
	case MeshType::HOLE_EX:
		if (inDetails->m_Rotation == 1)
			return true;
		break;
	case MeshType::HOLE_L:
		if (bLeft)
		{
			if (inDetails->m_Rotation == 2 || inDetails->m_Rotation == 3)
				return true;
		}
		else
		{
			if (inDetails->m_Rotation == 0 || inDetails->m_Rotation == 1)
				return true;
		}
		break;
	case MeshType::HOLE_T:
		if (bLeft && inDetails->m_Rotation != 3)
			return true;
		else if (!bLeft && inDetails->m_Rotation != 1)
			return true;
		break;
	default:
		break;
	}
	return false;
}

bool SplineTilePicker::GetAppropriateFirstTile(TArray<FIntVector>& possibleTiles, int numTilesPerRow)
{
	int currentIndex = TileLog.Num();

	if (CheckDependancyPrevious(currentIndex, numTilesPerRow)) // Means previous tile was a pit or hole
	{
		switch (TileLog[currentIndex - numTilesPerRow]->m_MeshCategory)
		{
		case MeshCategories::CATEGORY_PIT:
			// Generate tile that links with the previous pit tile and NOT left tile
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
			if (TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
			{
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 1 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 3 });
			}
			break;
		case MeshCategories::CATEGORY_HOLE:
			// Generate tile that links with the previous hole tile
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_EX, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 2 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 1 });
			if (TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_HOLE)
			{
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_4W, 0 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 1 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 0 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 3 });
			}
			break;
		}

		return true;
	}
	else if (numTilesPerRow > 1 && CheckDependancyPrevious(currentIndex + 1, numTilesPerRow))
	{
		// Next tile depends on previous add a left extender or independent tile
		// Independent tiles
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 1 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 2 });

		// Left Extenders
		switch (TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory)
		{
		case MeshCategories::CATEGORY_PIT:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
			break;
		case MeshCategories::CATEGORY_HOLE:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 2 });
			break;
		}
		
		return true;
	}
	else
	{
		// Generate random tile that DOESNT link to left or previous tile (unless its a hole)
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 1 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 3 });

		if (!CheckDependancyPrevious(currentIndex + 1, numTilesPerRow))
		{
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_L, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 2 });
		}
		else if (TileLog.Num() >= numTilesPerRow && TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
		{
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
		}
		else if (TileLog.Num() >= numTilesPerRow && TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_HOLE)
		{
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 2 });
		}

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
		switch (TileLog[currentIndex - 1]->m_MeshCategory)
		{
		case MeshCategories::CATEGORY_PIT:
			// generate a PIT that links with left and previous
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 2 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 1 });

			if (TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
			{
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 0 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_4W, 0 });
			}
			break;
		case MeshCategories::CATEGORY_HOLE:
			// generate a HOLE that links with left and previous
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 2 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 1 });
			if (TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_HOLE)
			{
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_4W, 0 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 0 });
			}
			break;
		}
		return true;
	}
	else if (bPrevious)
	{
		switch (TileLog[currentIndex - numTilesPerRow]->m_MeshCategory)
		{
		case MeshCategories::CATEGORY_PIT:
			// generate a tile that links with previous and NOT left
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
			if (TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
			{
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 1 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 3 });
			}
			break;
		case MeshCategories::CATEGORY_HOLE:
			// generate a tile that links with previous and NOT left
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_EX, 0 });
			if (TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_HOLE)
			{
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 1 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 3 });
			}
			break;
		}

		return true;
	}
	else if (bLeft)
	{
		// generate a tile that links with left and NOT previous
		switch (TileLog[currentIndex - 1]->m_MeshCategory)
		{
		case MeshCategories::CATEGORY_PIT:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 1 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 3 });
			if (TileLog.Num() >= numTilesPerRow && TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
			{
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 1 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 2 });
			}
			break;
		case MeshCategories::CATEGORY_RAMP:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_R, 0 });
			if (!CheckDependancyPrevious(currentIndex + 1, numTilesPerRow))
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_M, 0 });
			break;
		case MeshCategories::CATEGORY_HOLE:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 1 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 3 });
			if (TileLog.Num() >= numTilesPerRow && TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_HOLE)
			{
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_EX, 1 });
				possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 2 });
			}
			break;
		}

		return true;
	}
	else
	{
		// Generate random tile that DOESNT link to left or previous tile
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 2 });

		if (!CheckDependancyPrevious(currentIndex + 1, numTilesPerRow))
		{
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP_L, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 0 });
		}
		else if (TileLog.Num() >= numTilesPerRow && TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_PIT)
		{
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 0 });
		}
		else if (TileLog.Num() >= numTilesPerRow && TileLog[currentIndex + 1 - numTilesPerRow]->m_MeshCategory == MeshCategories::CATEGORY_HOLE)
		{
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 0 });
		}

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
		switch (TileLog[currentIndex - 1]->m_MeshCategory)
		{
		case MeshCategories::CATEGORY_PIT:
			// generate a tile that links with left and previous (We know these are two pits), DO NOT DEPEND ON RIGHT
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_L, 2 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_T, 1 });
			break;
		case MeshCategories::CATEGORY_HOLE:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 2 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 1 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_4W, 0 });
			break;
		}

		return true;
	}
	else if (bPrevious)
	{
		switch (TileLog[currentIndex - numTilesPerRow]->m_MeshCategory)
		{
		case MeshCategories::CATEGORY_PIT:
			// generate a tile that links with previous and NOT left (We know this is a pit), DO NOT DEPEND ON RIGHT
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_EX, 0 });
			break;
		case MeshCategories::CATEGORY_HOLE:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_EX, 0 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 1 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 3 });
			break;
		}

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
		case MeshCategories::CATEGORY_HOLE:
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 1 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 3 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_EX, 1 });
			possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_T, 2 });
			break;
		}

		return true;
	}
	else
	{
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_PIT, MeshType::PIT_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_RAMP, MeshType::RAMP, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE, 0 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 2 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_END_SN, 3 });
		possibleTiles.Emplace(FIntVector{ MeshCategories::CATEGORY_HOLE, MeshType::HOLE_L, 0 });

		return false;
	}
}

int SplineTilePicker::GetRandomIndexBasedOnWeight(TArray<FIntVector>& possibleTiles)
{
	TArray<int> weightMap;

	// Assign every option a weight. The higher the weight, the better the chance to get picked
	for (int i = 0; i < possibleTiles.Num(); i++)
	{
		if (bSpawnPits && possibleTiles[i].X == MeshCategories::CATEGORY_PIT)
		{
			switch (possibleTiles[i].Y)
			{
			case MeshType::PIT:
				weightMap.Emplace(WeightMap[MeshType::PIT]);
				break;
			case MeshType::PIT_4W:
				weightMap.Emplace(WeightMap[MeshType::PIT_4W]);
				break;
			case MeshType::PIT_END_SN:
				weightMap.Emplace(WeightMap[MeshType::PIT_END_SN]);
				break;
			case MeshType::PIT_EX:
				weightMap.Emplace(WeightMap[MeshType::PIT_EX]);
				break;
			case MeshType::PIT_L:
				weightMap.Emplace(WeightMap[MeshType::PIT_L]);
				break;
			case MeshType::PIT_T:
				weightMap.Emplace(WeightMap[MeshType::PIT_T]);
				break;
			default:
				break;
			}
		}
		else if (bSpawnRamps && possibleTiles[i].X == MeshCategories::CATEGORY_RAMP)
		{
			switch (possibleTiles[i].Y)
			{
			case MeshType::RAMP:
				weightMap.Emplace(WeightMap[MeshType::RAMP]);
				break;
			case MeshType::RAMP_L:
				weightMap.Emplace(WeightMap[MeshType::RAMP_L]);
				break;
			case MeshType::RAMP_M:
				weightMap.Emplace(WeightMap[MeshType::RAMP_M]);
				break;
			case MeshType::RAMP_R:
				weightMap.Emplace(WeightMap[MeshType::RAMP_R]);
				break;
			default:
				break;
			}
		}
		else if (bSpawnHoles && possibleTiles[i].X == MeshCategories::CATEGORY_HOLE)
		{
			switch (possibleTiles[i].Y)
			{
			case MeshType::HOLE:
				weightMap.Emplace(WeightMap[MeshType::HOLE]);
				break;
			case MeshType::HOLE_4W:
				weightMap.Emplace(WeightMap[MeshType::HOLE_4W]);
				break;
			case MeshType::HOLE_END_SN:
				weightMap.Emplace(WeightMap[MeshType::HOLE_END_SN]);
				break;
			case MeshType::HOLE_EX:
				weightMap.Emplace(WeightMap[MeshType::HOLE_EX]);
				break;
			case MeshType::HOLE_L:
				weightMap.Emplace(WeightMap[MeshType::HOLE_L]);
				break;
			case MeshType::HOLE_T:
				weightMap.Emplace(WeightMap[MeshType::HOLE_T]);
				break;
			default:
				break;
			}
		}
		else
			weightMap.Emplace(0);
	}

	int sumWeight = 0;

	// Calculate the sum of the weights
	for (int i = 0; i < weightMap.Num(); i++)
	{
		sumWeight += weightMap[i];
	}

	int sumRandom = FMath::RandRange(0, sumWeight - 1);

	// Iterate through the map, subtracting the current item's weight from the random number
	// untill one is picked.
	for (int i = 0; i < weightMap.Num(); i++)
	{
		if(sumRandom < weightMap[i])
			return i;
		sumRandom -= weightMap[i];
	}

	//UE_LOG(LogTemp, Warning, TEXT("Failed to randomly select a tile"));
	// Returning -1 will spawn a default tile
	return -1;
}

void SplineTilePicker::AddEdgeMesh(TArray<UStaticMesh*>& tileMesh, int numTilesPerRow)
{
	UStaticMesh* edgeMeshL,* edgeMeshR;
	TileDetails* edgeDetailsL = new TileDetails,* edgeDetailsR = new TileDetails;
	edgeDetailsL->m_MeshCategory = MeshCategories::CATEGORY_EDGE;
	edgeDetailsR->m_MeshCategory = MeshCategories::CATEGORY_EDGE;
	edgeDetailsL->m_Rotation = 0;
	edgeDetailsR->m_Rotation = 0;

	if (CheckNeedSpecificEdge(TileLog[TileLog.Num() - numTilesPerRow], true))
	{
		edgeDetailsL->m_MeshType = EdgeMeshType::EDGE_HOLE_L;
		//TileLog.EmplaceAt(TileLog.Num() - numTilesPerRow, edgeDetailsL);

		edgeMeshL = MeshLib->GetEdgeMesh(edgeDetailsL->m_MeshType);
		tileMesh.EmplaceAt(0, edgeMeshL);
	}
	else
	{
		edgeDetailsL->m_MeshType = EdgeMeshType::EDGE_DIVE_L;
		//TileLog.EmplaceAt(TileLog.Num() - numTilesPerRow, edgeDetailsL);

		edgeMeshL = MeshLib->GetEdgeMesh(edgeDetailsL->m_MeshType);
		tileMesh.EmplaceAt(0, edgeMeshL);
	}
	if (CheckNeedSpecificEdge(TileLog[TileLog.Num() - 1], false))
	{
		edgeDetailsR->m_MeshType = EdgeMeshType::EDGE_HOLE_R;
		//TileLog.Emplace(edgeDetailsR);

		edgeMeshR = MeshLib->GetEdgeMesh(edgeDetailsR->m_MeshType);
		tileMesh.Emplace(edgeMeshR);
	}
	else
	{
		edgeDetailsR->m_MeshType = EdgeMeshType::EDGE_DIVE_R;
		//TileLog.Emplace(edgeDetailsR);

		edgeMeshR = MeshLib->GetEdgeMesh(edgeDetailsR->m_MeshType);
		tileMesh.Emplace(edgeMeshR);
	}
}