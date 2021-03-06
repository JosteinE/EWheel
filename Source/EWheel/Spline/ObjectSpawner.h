// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ObjectSpawner.generated.h"

struct TileDetails;
class AObjectActorBase;
class UStaticMesh;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EWHEEL_API UObjectSpawner : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObjectSpawner();
	~UObjectSpawner();

	// Checks for spawn conditions and spawn appropriate objects based on their spawn chance
	// Wanted to pass the arrays as ptrs or references, but it kept losing the data spawning stuff in weird places
	void CheckAndSpawnObjectsOnNewestTiles(TArray<TileDetails*>* TileLog, TArray<FVector>& tileLocations, FRotator& tileRotations);
	
	// Spawn objects
	AObjectActorBase* SpawnObstacleActor(FVector& location, FRotator& rotation);
	AObjectActorBase* SpawnPickUpActor(FVector& location, FRotator& rotation);
	AObjectActorBase* SpawnPowerUpActor(FVector& location, FRotator& rotation);

	// Removes the objects that belonged to a row that has been removed
	void CheckAndRemoveObjectsFromLastRow();

private:
	// Obstacle Mesh Library
	TArray<UStaticMesh*> mLibrary;

	// Map of objects and their belonging row
	TMap<int, TArray<AObjectActorBase*>> mObjects;
	int mRowTracker = 0;
	int mMaxRows = 0;

	// Object Spawner chances (in percentages)
	int mObstacleSpawnChance = 33;
	int mPointSpawnChance = 8;
	int mPowerUpSpawnChance = 1;
	bool bSkipEveryOtherRow = true;
	bool bSpawnedObstaclesOnLastRow = false;

public:
	FORCEINLINE void SetMaxRows(int maxRows) { mMaxRows = maxRows; };
	FORCEINLINE void SetObstacleSpawnChance(int obsSpawnChance) { mObstacleSpawnChance = obsSpawnChance; };
	FORCEINLINE void SetPointSpawnChance(int pointSpawnChance) { mPointSpawnChance = pointSpawnChance; };
	FORCEINLINE void SetPowerUpSpawnChance(int powerUpSpawnChance) { mPowerUpSpawnChance = powerUpSpawnChance; };
};
