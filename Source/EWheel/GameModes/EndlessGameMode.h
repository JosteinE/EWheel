// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "EWheel/MeshGenerator.h"

#include "EndlessGameMode.generated.h"
/**
 * 
 */
class AObstacleActor;
class APickUpActor;
class AMeshSplineActor;
UCLASS()
class EWHEEL_API AEndlessGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AEndlessGameMode();

private:
	void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void ExtendPath();
	// 0 = left, 1 = middle, 2 = right
	FVector GetTileCentreLastRow(int index);
	void SpawnPointObject(FVector &location);
	void SpawnObstacleObject(FVector& location);

public:
	void EndGame();

private:
	APawn* mainPlayer;
	AMeshSplineActor* mainPath;
	APlayerController* mainPlayerController;

	// Number of tiles per row
	int TilesPerRow = 3;

	float splineSpawnVerticalOffset = -50.f;
	// Add a new point if the player is within this range to the last spline point
	float minDistToSplinePoint = 225.f;
	
	// Distance to the next spline point;
	float distToNextSplinePoint = 300.f;
	
	// Max number of spline & mesh points along the curve
	int maxNumSplinePoints = 9;
	// 
	int extendFromSplinePoint = 3;
	FVector lastSplinePointLoc;

	//
	TArray<APickUpActor*> PickupActors;
	TArray<AObstacleActor*> ObstacleActors;
	UStaticMesh* PointObjectMesh;
	UStaticMesh* ObstacleMesh;

	// Deletes the first pickup actor in the PickupActors array when this number is reached (should be less than maxNumSplinePoints * TilesPerRow)
	int maxNumPickups = maxNumSplinePoints * TilesPerRow;
	// Deletes the first obstacle actor in the ObstacleMesh array when this number is reached (should be less than maxNumSplinePoints * TilesPerRow)
	int maxNumObstacles = maxNumSplinePoints * TilesPerRow;
	// Test
	MeshGenerator meshGen;
	TArray<FString> meshPathLib;

	int32 PointSpawnChance = 8;
	int32 ObstacleSpawnChance = 25;
};
