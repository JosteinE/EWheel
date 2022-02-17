// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/ObjectSpawner.h"
#include "EWheel/Spline/MeshCategoriesAndTypes.h"
#include "EWheel/Spline/TileDetails.h"
#include "EWheel/Objects/PickUpActor.h"
#include "EWheel/Objects/ObstacleActor.h"


// Sets default values for this component's properties
UObjectSpawner::UObjectSpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UObjectSpawner::CheckAndSpawnObjectsOnNewestTiles(TArray<TileDetails*>* TileLog)
{
	mRowTracker++;

	int tilesPerRow = TileLog->Num() * 0.5f;
	TArray<int> possibleTilesIndices;
	for (int i = 0; i < tilesPerRow; i++)
	{
		possibleTilesIndices.Emplace(i);
	}

	// Spawn point objects (regardless of hole and ramp)
	for (int i = 0; i < possibleTilesIndices.Num(); i++)
	{
		if (FMath::RandRange(0, 99) < mPointSpawnChance)
		{
			SpawnPickUpActor();
			// Spawn pointobject
			// SetObjectTileIndex = possibleTilesIndices[i]
			possibleTilesIndices.RemoveAt(i);
		}
	}

	// Check for obstacles on previous row. Don't spawn an obstacle directly after
	if (mObjects.Contains(mRowTracker - 1))
	{
		// Get indices of previous obstacle tiles
		TArray<int> obstacleTiles;
		for (int i = 0; i < mObjects[mRowTracker - 1].Num(); i++)
		{
			if (Cast<AObstacleActor>(mObjects[mRowTracker - 1][i]))
			{
				obstacleTiles.Emplace(i);
			}
		}

		// If there was only one available tile on the last row remove the following as a possible index
		if (obstacleTiles.Num() == tilesPerRow - 1)
		{
			for (int i = 0; i < tilesPerRow; i++)
			{
				if (possibleTilesIndices[i] != obstacleTiles[i])
				{
					possibleTilesIndices.RemoveAt(i);
					break;
				}
			}
		}
	}


	// Check where there's a valid tile on the new row for obstacles (no holes or ramps)
	for (int i = tilesPerRow; i < TileLog->Num(); i++)
	{
		if ((*TileLog)[i]->m_MeshCategory == MeshCategories::CATEGORY_HOLE && (*TileLog)[i]->m_MeshCategory == MeshCategories::CATEGORY_RAMP)
		{
			possibleTilesIndices.RemoveAt(i - tilesPerRow);
		}
	}

	// Spawn obstacle if there are more than one remaining empty tile
	if (possibleTilesIndices.Num() > 1)
	{
		int obstaclesSpawned = 0;
		for (int i = 0; i < possibleTilesIndices.Num(); i++)
		{
			if (FMath::RandRange(0, 99) < mPointSpawnChance)
			{
				// Spawn pointobject
				// SetObjectTileIndex = possibleTilesIndices[i]
			}

			// Break if there's only one valid tile left without an obstacle
			if (obstaclesSpawned >= possibleTilesIndices.Num() - 1)
				break;
		}
	}
}

void UObjectSpawner::SpawnObstacleActor(FVector& location)
{
}

void UObjectSpawner::SpawnPickUpActor(FVector& location)
{
	FActorSpawnParameters ObjectSpawnParams;
	ObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APickUpActor* PickUpActor = GetWorld()->SpawnActor<APickUpActor>(APickUpActor::StaticClass(), FVector(), FRotator(), ObjectSpawnParams);
	
	mObjects.Emplace(PickUpActor);
	
	PickUpActor->SetStaticMesh(PointObjectMesh);
	
	PickUpActor->SetActorLocation(location + FVector{ 0.f, 0.f, 50.f });
	PickUpActor->GetMeshComponent()->SetWorldScale3D(FVector{ 0.33f, 0.33f, 0.33f });
	PickUpActor->GetMeshComponent()->SetRelativeRotation(FRotator{ 90.f, 0.f, 0.f });
//PointObject->GetMeshComponent()->RegisterComponent();
}

void UObjectSpawner::SpawnPowerUpActor(FVector& location)
{
}

void UObjectSpawner::CheckAndRemoveObjectsFromRow(int RowIndex)
{
	if (mObjects.Contains(RowIndex))
	{
		for (int i = 0; i < mObjects[RowIndex].Num(); i++)
		{
			mObjects[RowIndex][0]->Destroy();
			mObjects[RowIndex][0] = nullptr;
			mObjects[RowIndex].RemoveAt(0);
		}
		mObjects[RowIndex].Empty();
		mObjects.Remove(RowIndex);
	}
}

void UObjectSpawner::CheckAndRemoveObjects()
{
}
