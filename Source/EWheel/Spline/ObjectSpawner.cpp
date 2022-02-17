// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/ObjectSpawner.h"
#include "EWheel/Spline/MeshCategoriesAndTypes.h"
#include "EWheel/Spline/TileDetails.h"
#include "EWheel/Objects/PickUpActor.h"
#include "EWheel/Objects/ObstacleActor.h"
#include "Engine/StaticMesh.h"


// Sets default values for this component's properties
UObjectSpawner::UObjectSpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	TArray<FString> meshPaths;

	// PickUps
	meshPaths.Emplace("StaticMesh'/Game/Meshes/PointObject.PointObject'");

	// Obstacles
	meshPaths.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_BigRoot_150x150.Obstacle_BigRoot_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_Log_150x150.Obstacle_Log_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_RampStone_150x150.Obstacle_RampStone_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_Stone_150x150.Obstacle_Stone_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_Stump_150x150.Obstacle_Stump_150x150'");

	for (int i = 0; i < meshPaths.Num(); i++)
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(*meshPaths[i]);
		if (MeshAsset.Succeeded())
			mLibrary.Emplace(Cast<UStaticMesh>(MeshAsset.Object));
	}
}

UObjectSpawner::~UObjectSpawner()
{
	//for (TPair<int, TArray<AObjectActorBase*>> rowObjects : mObjects)
	//{
	//	for (int i = 0; i < rowObjects.Value.Num(); i++)
	//	{
	//		if(IsValid(rowObjects.Value[i]))
	//			rowObjects.Value[i]->Destroy();
	//		rowObjects.Value.RemoveAt(i, 0, false);
	//	}
	//	rowObjects.Value.Empty();
	//	mObjects.Remove(rowObjects.Key);
	//}
}

void UObjectSpawner::CheckAndSpawnObjectsOnNewestTiles(TArray<TileDetails*>* TileLog, TArray<FVector>& tileLocations, TArray<FRotator>& tileRotations)
{
	// Make an array to store potential tile indices for objects. Assumes that tilelog stores 2 rows
	int tilesPerRow = TileLog->Num() * 0.5f;
	TArray<int> possibleTilesIndices;
	for (int i = 0; i < tilesPerRow; i++)
	{
		possibleTilesIndices.Emplace(i);
	}

	int numPointObjectsSpawned = 0;
	// Spawn point objects (regardless of hole and ramp)
	for (int i = possibleTilesIndices.Num() - 1; i >= 0; i--)
	{
		if (FMath::RandRange(0, 99) < mPointSpawnChance)
		{
			AObjectActorBase* newPickup = SpawnPickUpActor(tileLocations[possibleTilesIndices[i]], tileRotations[possibleTilesIndices[i]]);
			newPickup->mTileIndex = possibleTilesIndices[i];
			numPointObjectsSpawned++;
			possibleTilesIndices.RemoveAt(i);
		}
	}

	possibleTilesIndices.Shrink();

	// Check for obstacles on previous row. Don't spawn an obstacle directly after
	if (mObjects.Contains(mRowTracker - 1))
	{
		// Get indices of previous obstacle tiles
		TArray<int> obstacleTiles;
		for (int i = 0; i < mObjects[mRowTracker - 1].Num(); i++)
		{
			if (mObjects[mRowTracker - 1][i]->IsA(AObstacleActor::StaticClass()))
			{
				obstacleTiles.Emplace(i);
			}
		}

		// If there was only one available tile on the last row remove the following as a possible index
		if (obstacleTiles.Num() == tilesPerRow - 1)
		{
			bool removedLastEmptyTile = false;
			for (int i = possibleTilesIndices.Num() - 1; i >= 0; i--)
			{
				for (int ii = obstacleTiles.Num() - 1; ii >= 0; ii--)
				{
					if (possibleTilesIndices[i] != obstacleTiles[ii])
					{
						possibleTilesIndices.RemoveAt(i);
						removedLastEmptyTile = true;
						break;
					}
				}
				if (removedLastEmptyTile)
					break;
			}
		}
	}


	if (possibleTilesIndices.Num() > 1)
	{
		// Check where there's a valid tile on the new row for obstacles (no holes or ramps)
		for (int i = possibleTilesIndices.Num() - 1; i >= 0; i--)
		{
			for (int ii = TileLog->Num() - 1; ii >= tilesPerRow; ii--)
			{ // ((*(*TileLog)[ii]).m_MeshCategory == MeshCategories::CATEGORY_HOLE || (*(*TileLog)[ii]).m_MeshCategory == MeshCategories::CATEGORY_RAMP
				if (possibleTilesIndices[i] == ii - tilesPerRow && ((*TileLog)[ii]->m_MeshCategory == MeshCategories::CATEGORY_HOLE || (*TileLog)[ii]->m_MeshCategory == MeshCategories::CATEGORY_RAMP))
				{
					possibleTilesIndices.RemoveAt(i);
					break;
				}
			}
		}
	}

	// Spawn obstacle if there are more than one remaining empty tile
	if (possibleTilesIndices.Num() > 1)
	{
		int obstaclesSpawned = 0;
		for (int i = 0; i < possibleTilesIndices.Num(); i++)
		{
			if (FMath::RandRange(0, 99) < mObstacleSpawnChance)
			{
				AObjectActorBase* newObstacle = SpawnObstacleActor(tileLocations[possibleTilesIndices[i]], tileRotations[possibleTilesIndices[i]]);
				newObstacle->mTileIndex = possibleTilesIndices[i];
				obstaclesSpawned++;
			}

			// Break if there's only one valid tile left without an obstacle
			if (obstaclesSpawned >= possibleTilesIndices.Num() + numPointObjectsSpawned - 1)
				break;
		}
	}

	// Update the row tracker to index the new objects
	mRowTracker++;
	if (mRowTracker > mMaxRows)
		mRowTracker = 0;
}

AObjectActorBase* UObjectSpawner::SpawnObstacleActor(FVector& location, FRotator& rotation)
{
	FActorSpawnParameters ObjectSpawnParams;
	ObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AObstacleActor* ObstacleObject = GetWorld()->SpawnActor<AObstacleActor>(AObstacleActor::StaticClass(), FVector(), FRotator(), ObjectSpawnParams);
	
	if (!mObjects.Contains(mRowTracker))
		mObjects.Add(mRowTracker);
	mObjects[mRowTracker].Emplace(ObstacleObject);
	
	ObstacleObject->SetStaticMesh(mLibrary[FMath::RandRange(1, 5)]);
	
	float tempHeight = FMath::RandRange(0.5f, 1.f);
	ObstacleObject->SetActorLocation(location);
	ObstacleObject->GetMeshComponent()->SetWorldRotation(rotation + FRotator{ 0.f, 180.f, 0.f });
//ObstacleObject->GetMeshComponent()->SetWorldScale3D(FVector{ FMath::RandRange(0.75f, 1.5f), FMath::RandRange(0.75f, 1.5f), FMath::RandRange(0.1f, 1.f) * tempHeight });
//ObstacleObject->GetMeshComponent()->SetRelativeRotation(FRotator{ 0.f, FMath::RandRange(0.f, 90.f), 0.f });
//PointObject->GetMeshComponent()->RegisterComponent();

	return ObstacleObject;
}

AObjectActorBase* UObjectSpawner::SpawnPickUpActor(FVector& location, FRotator& rotation)
{
	FActorSpawnParameters ObjectSpawnParams;
	ObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APickUpActor* PickUpActor = GetWorld()->SpawnActor<APickUpActor>(APickUpActor::StaticClass(), FVector(), FRotator(), ObjectSpawnParams);
	
	if(!mObjects.Contains(mRowTracker))
		mObjects.Add(mRowTracker);
	mObjects[mRowTracker].Emplace(PickUpActor);
	
	PickUpActor->SetStaticMesh(mLibrary[0]);
	
	PickUpActor->SetActorLocation(location + FVector{ 0.f, 0.f, 50.f });
	PickUpActor->GetMeshComponent()->SetWorldScale3D(FVector{ 0.33f, 0.33f, 0.33f });
	PickUpActor->GetMeshComponent()->SetRelativeRotation(FRotator{ 90.f, 0.f, 0.f });
//PointObject->GetMeshComponent()->RegisterComponent();

	return PickUpActor;
}

AObjectActorBase* UObjectSpawner::SpawnPowerUpActor(FVector& location, FRotator& rotation)
{
	return nullptr;
}

void UObjectSpawner::CheckAndRemoveObjectsFromLastRow()
{
	if (mObjects.Contains(mRowTracker))
	{
		for (int i = 0; i < mObjects[mRowTracker].Num(); i++)
		{
			mObjects[mRowTracker][i]->Destroy();
			mObjects[mRowTracker][i] = nullptr;
			mObjects[mRowTracker].RemoveAt(i, 0, false);
		}
		mObjects[mRowTracker].Empty();
		mObjects.Remove(mRowTracker);
	}
}
