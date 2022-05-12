// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/ObjectSpawner.h"
#include "EWheel/Spline/MeshCategoriesAndTypes.h"
#include "EWheel/Spline/TileDetails.h"
#include "EWheel/Objects/PickUpActor.h"
#include "EWheel/Objects/ObstacleCube.h"
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

	//meshPaths.Emplace("StaticMesh'/Game/Stylized_Forest/Meshes/stones/SM_small_stone_04.SM_small_stone_04'"); // .8 
	//meshPaths.Emplace("StaticMesh'/Game/Stylized_Forest/Meshes/stones/SM_stone_03.SM_stone_03'"); // .43
	//meshPaths.Emplace("StaticMesh'/Game/Stylized_Forest/Meshes/stones/SM_stone_02.SM_stone_02'"); // .4
	//meshPaths.Emplace("StaticMesh'/Game/Stylized_Forest/Meshes/trees/SM_stump_01.SM_stump_01'"); // .66
	//meshPaths.Emplace("StaticMesh'/Game/Stylized_Forest/Meshes/trees/SM_stump_02.SM_stump_02'"); // .66
	//meshPaths.Emplace("StaticMesh'/Game/Stylized_Forest/Meshes/trees/SM_stump_03.SM_stump_03'"); // .33
	//meshPaths.Emplace("StaticMesh'/Game/Stylized_Forest/Meshes/trees/SM_log_01.SM_log_01'"); // .28
	//meshPaths.Emplace("StaticMesh'/Game/Stylized_Forest/Meshes/trees/SM_branch_02.SM_branch_02'"); // .18

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

void UObjectSpawner::CheckAndSpawnObjectsOnNewestTiles(TArray<TileDetails*>* TileLog, TArray<FVector>& tileLocations, FRotator& tileRotations)
{
	if (bSpawnedObstaclesOnLastRow && bSkipEveryOtherRow)
	{
		bSpawnedObstaclesOnLastRow = false;
		mRowTracker++;
		if (mRowTracker > mMaxRows)
			mRowTracker = 0;
		return;
	}

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
			AObjectActorBase* newPickup = SpawnPickUpActor(tileLocations[possibleTilesIndices[i]], tileRotations);
			newPickup->mTileIndex = possibleTilesIndices[i];
			numPointObjectsSpawned++;
			possibleTilesIndices.RemoveAt(i);
		}
	}

	// Check for obstacles on previous row. Don't spawn an obstacle directly after an open tile if that tile was the only one without obstacles
	bool bSavedLastEmptyTile = false;
	if (mObjects.Contains(mRowTracker - 1 - bSkipEveryOtherRow))
	{
		// Get indices of previous obstacle tiles
		TArray<int> obstacleTiles;
		for (int i = 0; i < mObjects[mRowTracker - 1 - bSkipEveryOtherRow].Num(); i++)
		{
			if (mObjects[mRowTracker - 1 - bSkipEveryOtherRow][i]->IsA(AObstacleActor::StaticClass()))
			{
				obstacleTiles.Emplace(i);
			}
		}

		// If there was only one available tile on the last row remove the following as a possible index
		if (obstacleTiles.Num() == tilesPerRow - 1)
		{
			for (int i = possibleTilesIndices.Num() - 1; i >= 0; i--)
			{
				for (int ii = obstacleTiles.Num() - 1; ii >= 0; ii--)
				{
					if (possibleTilesIndices[i] != obstacleTiles[ii])
					{
						possibleTilesIndices.RemoveAt(i);
						bSavedLastEmptyTile = true;
						break;
					}
				}
				if (bSavedLastEmptyTile)
					break;
			}
		}
	}

	int numBadTiles = 0;
	if (possibleTilesIndices.Num() > 0)
	{
		// Check where there's a valid tile on the new row for obstacles (no holes or ramps)
		for (int i = possibleTilesIndices.Num() - 1; i >= 0; i--)
		{
			for (int ii = TileLog->Num() - 1; ii >= tilesPerRow; ii--)
			{ // ((*TileLog)[ii].m_MeshCategory == MeshCategories::CATEGORY_HOLE || (*TileLog)[ii].m_MeshCategory == MeshCategories::CATEGORY_RAMP
				if (possibleTilesIndices[i] == ii - tilesPerRow)
				{
					if ((*TileLog)[ii]->m_MeshCategory == MeshCategories::CATEGORY_HOLE)
					{
						possibleTilesIndices.RemoveAt(i);
						break;
					}
					else if((*TileLog)[ii]->m_MeshCategory == MeshCategories::CATEGORY_RAMP)
					{
						possibleTilesIndices.RemoveAt(i);
						numBadTiles++;
						break;
					}
				}
			}
		}
	}

	// Spawn obstacle if there are more than one remaining empty tile
	if (possibleTilesIndices.Num() + numPointObjectsSpawned + numBadTiles > 0)
	{
		int obstaclesSpawned = 0;
		for (int i = 0; i < possibleTilesIndices.Num(); i++)
		{
			if (FMath::RandRange(0, 99) < mObstacleSpawnChance)
			{
				AObjectActorBase* newObstacle = SpawnObstacleActor(tileLocations[possibleTilesIndices[i]], tileRotations);
				newObstacle->mTileIndex = possibleTilesIndices[i];
				obstaclesSpawned++;
				bSpawnedObstaclesOnLastRow = true;
			}

			// Break if there's only one valid tile left without an obstacle
			if (obstaclesSpawned >= tilesPerRow - 1)
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
		
	AObstacleActor* ObstacleObject = GetWorld()->SpawnActor<AObstacleActor>(AObstacleActor::StaticClass(), location, FRotator{ 0.f, FMath::RandRange(0.f, 359.f), 0.f }, ObjectSpawnParams);
	//AObstacleActor* ObstacleObject = GetWorld()->SpawnActor<AObstacleActor>(AObstacleCube::StaticClass(), location, rotation, ObjectSpawnParams);

	if (!mObjects.Contains(mRowTracker))
		mObjects.Add(mRowTracker);
	mObjects[mRowTracker].Emplace(ObstacleObject);
	
	int objIndex = FMath::RandRange(1, mLibrary.Num() - 1);
	ObstacleObject->SetStaticMesh(mLibrary[objIndex]);

	//switch (objIndex)
	//{
	//case 1:
	//	ObstacleObject->SetActorScale3D(FVector{ 0.8f });
	//	break;
	//case 2:
	//	ObstacleObject->SetActorScale3D(FVector{ 0.43f });
	//	break;
	//case 3:
	//	ObstacleObject->SetActorScale3D(FVector{ 0.4f });
	//	break;
	//case 4:
	//	ObstacleObject->SetActorScale3D(FVector{ 0.66f });
	//	break;
	//case 5:
	//	ObstacleObject->SetActorScale3D(FVector{ 0.66f });
	//	break;
	//case 6:
	//	ObstacleObject->SetActorScale3D(FVector{ 0.33f });
	//	break;
	//case 7:
	//	ObstacleObject->SetActorScale3D(FVector{ 0.28f });
	//	break;
	//case 8:
	//	ObstacleObject->SetActorScale3D(FVector{ 0.18f });
	//	break;
	//default:
	//	break;
	//}

	//ObstacleObject->SetActorScale3D(FVector{ 0.33f });
	//Cast<AObstacleCube>(ObstacleObject)->SetHeight(FMath::RandRange(0.f, 25.f));

	return ObstacleObject;
}

AObjectActorBase* UObjectSpawner::SpawnPickUpActor(FVector& location, FRotator& rotation)
{
	FActorSpawnParameters ObjectSpawnParams;
	ObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APickUpActor* PickUpActor = GetWorld()->SpawnActor<APickUpActor>(APickUpActor::StaticClass(), location + FVector{ 0.f, 0.f, 50.f }, FRotator{ 90.f, 0.f, 0.f }, ObjectSpawnParams);
	
	if(!mObjects.Contains(mRowTracker))
		mObjects.Add(mRowTracker);
	mObjects[mRowTracker].Emplace(PickUpActor);
	
	PickUpActor->SetStaticMesh(mLibrary[0]);
	
	//PickUpActor->SetActorLocation(location + FVector{ 0.f, 0.f, 50.f });
	PickUpActor->GetMeshComponent()->SetWorldScale3D(FVector{ 0.33f, 0.33f, 0.33f });
	//PickUpActor->GetMeshComponent()->SetRelativeRotation(FRotator{ 90.f, 0.f, 0.f });

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
		}
		mObjects[mRowTracker].Empty();
		mObjects.Remove(mRowTracker);
	}
}
