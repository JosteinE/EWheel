// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

//TempForTetst
#include "Engine/StaticMesh.h"

#include "EndlessGameMode.generated.h"
/**
 * 
 */
class AMeshSplineActor;
UCLASS()
class EWHEEL_API AEndlessGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AEndlessGameMode();

private:
	void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	void EndGame();

private:
	APawn* mainPlayer;
	AMeshSplineActor* mainPath;
	APlayerController* mainPlayerController;

	float splineSpawnVerticalOffset = 10.f;
	// Add a new point if the player is within this range to the last spline point
	float minDistToLastSplinePoint = 100.f;
	
	// Distance to the next spline point;
	float distToNextSplinePoint = 200.f;
	
	// Max number of spline & mesh points along the curve
	int maxNumSplinePoints = 5;
	FVector lastSplinePointLoc;

	AActor* testMesh;
};
