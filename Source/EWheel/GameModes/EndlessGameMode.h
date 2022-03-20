// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

//#include "EWheel/MeshGenerator.h"

#include "EndlessGameMode.generated.h"
/**
 * 
 */
struct HighscoreSlot;
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

	UFUNCTION(BlueprintCallable)
	void WriteToHighscores(UPARAM(ref)FString& playerName);

	UFUNCTION(Blueprintcallable)
	void GetGameTimeString(FString& returnString);

	UFUNCTION(Blueprintcallable)
	void GetGameModeStringFromInt(FString& returnString, int mode);

	// Current Game mode, set in the main menu
	UPROPERTY(Category = GameMode, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int mGameMode = 0;

private:

	void CalculateChaseBoxSpeed();

	// Checks if the player is getting too close to the end
	bool CheckShouldExtend();

	APawn* mainPlayer;
	TSharedPtr<HighscoreSlot> mSlot;
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
	int extendFromSplinePoint = 10;

	// Material to be applied to the path mesh
	UMaterialInterface* DefaultMaterial;

	TSubclassOf<AActor> ChaseBoxClass;
	AActor* StartChaseBox;
	float StartChaseBoxSplineIndex = 0.5f;
	float StartChaseBoxSpeed = 150.f;
	float ChaseBoxTimeToMaxSpeed = 5.f;
	float ChaseBoxMaxSpeed; // Is set equal to player in begin play
	AActor* ChaseBoxEnd;
	float ChaseBoxDistanceThreshold = 10;

	bool bSpawnedObstacleOnLast = false;
};
