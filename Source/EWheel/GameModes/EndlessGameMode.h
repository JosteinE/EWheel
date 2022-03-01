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

public:
	UFUNCTION(BlueprintCallable)
	void EndGame();

	UFUNCTION()
	void OnPlayerEscapePressed();

	UFUNCTION()
	void OnPlayerRestartPressed();

	UFUNCTION()
	void OnPlayerDeath();

	// Current Game mode, set in the main menu
	UPROPERTY(Category = GameMode, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int mGameMode = 0;

private:
	void GetGameModeStringFromInt(FString& returnString, int mode);

	APawn* mainPlayer;
	AMeshSplineMaster* mPathMaster;

	// Number of tiles per row
	int mNumSplines = 3;
	int TileSize = 150;

	// Add a new point if the player is within this range to the last spline point
	float minDistToSplinePoint = 225.f;
	
	// Distance to the next spline point (Should be =TileSize)
	float distToNextSplinePoint = 150.f;

	// Max number of spline points along the curve (NB! the curve needs a leading point, so the total number is maxNumSplinePoints + 1)
	int maxNumSplinePoints = 20;
	
	// Tile index to extend from 
	int extendFromSplinePoint = 5;

	// Material to be applied to the path mesh
	UMaterialInterface* DefaultMaterial;

	bool bSpawnedObstacleOnLast = false;
};
