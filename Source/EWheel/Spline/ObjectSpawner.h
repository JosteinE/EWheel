// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ObjectSpawner.generated.h"

class SplineTilePicker;
struct TileDetails;
class AObstacleActor;
class APickUpActor;
class AObjectActorBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EWHEEL_API UObjectSpawner : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObjectSpawner();

	// Checks for spawn conditions and spawn appropriate objects based on their spawn chance
	void CheckAndSpawnObjectsOnNewestTiles(TArray<TileDetails*>* TileLog);
	
	// Spawn objects
	void SpawnObstacleActor(FVector& location);
	void SpawnPickUpActor(FVector& location);
	void SpawnPowerUpActor(FVector& location);

	// Removes the objects that belonged to a row that has been removed
	void CheckAndRemoveObjectsFromRow(int RowIndex);

private:
	// Map of objects and their belonging row
	TMap<int, TArray<AObjectActorBase*>> mObjects;
	int mRowTracker = 0;

	// Object Spawner chances (in percentages)
	int mObstacleSpawnChance = 33;
	int mPointSpawnChance = 8;
	int mPowerUpSpawnChance = 1;
};
