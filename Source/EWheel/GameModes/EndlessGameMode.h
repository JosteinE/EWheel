// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

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
	void ExtendPath();
	void SpawnPointObject(FVector &location);

public:
	void EndGame();

private:
	APawn* mainPlayer;
	AMeshSplineActor* mainPath;
	APlayerController* mainPlayerController;

	float splineSpawnVerticalOffset = -50.f;
	// Add a new point if the player is within this range to the last spline point
	float minDistToLastSplinePoint = 100.f;
	
	// Distance to the next spline point;
	float distToNextSplinePoint = 300.f;
	
	// Max number of spline & mesh points along the curve
	int maxNumSplinePoints = 20;
	FVector lastSplinePointLoc;

	int tileSpawnedCounter = 0;
	AActor* PointObject;
	UStaticMeshComponent* PointObjectMeshComponent;
	UStaticMesh* PointObjectMesh;
};
