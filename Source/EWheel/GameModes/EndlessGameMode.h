// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

//#include "EWheel/MeshGenerator.h"

#include "EndlessGameMode.generated.h"
/**
 * 
 */
class AObstacleActor;
class APickUpActor;
class AMeshSplineMaster;

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
	UFUNCTION(BlueprintCallable)
	void EndGame();

	UFUNCTION()
	void OnPlayerEscapePressed();

	UFUNCTION()
	void OnPlayerRestartPressed();

	UFUNCTION()
	void OnPlayerDeath();
private:
	APawn* mainPlayer;
	AMeshSplineMaster* mPathMaster;
	APlayerController* mainPlayerController;

	// Number of tiles per row
	int mNumSplines = 3;
	int TileSize = 150;

	float playerSpawnHeight = 100.f;
	float splineSpawnVerticalOffset = 0.f;
	float splineVerticalMin = -50.f; // -50
	float splineVerticalMax = 50.f; // 50
	float splineVerticalStep = 20.f;
	// Add a new point if the player is within this range to the last spline point
	float minDistToSplinePoint = 225.f;
	
	// Distance to the next spline point (Should be =TileSize)
	float distToNextSplinePoint = 150.f;

	// Max number of spline points along the curve (NB! the curve needs a leading point, so the total number is maxNumSplinePoints+1)
	int maxNumSplinePoints = 20;
	// 
	int extendFromSplinePoint = 5;
	FVector lastSplinePointLoc;

	//
	TArray<APickUpActor*> PickupActors;
	TArray<AObstacleActor*> ObstacleActors;
	UStaticMesh* PointObjectMesh;
	UStaticMesh* ObstacleMesh;
	UMaterialInterface* DefaultMaterial;

	// Deletes the first pickup actor in the PickupActors array when this number is reached (should be less than maxNumSplinePoints * TilesPerRow)
	int maxNumPickups = maxNumSplinePoints * mNumSplines;
	// Deletes the first obstacle actor in the ObstacleMesh array when this number is reached (should be less than maxNumSplinePoints * TilesPerRow)
	int maxNumObstacles = maxNumSplinePoints * mNumSplines;
	// Test
	//MeshGenerator meshGen;
	TArray<FString> meshPathLib;
	
	//TSubclassOf<class APlayerPawn> pawnClass = nullptr;

	int32 PointSpawnChance = 8;
	int32 ObstacleSpawnChance = 33;
	bool bSpawnedObstacleOnLast = false;
};
