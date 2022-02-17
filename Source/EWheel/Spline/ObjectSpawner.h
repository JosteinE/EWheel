// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ObjectSpawner.generated.h"

class SplineTilePicker;
struct TileDetails;
class AObstacleActor;
class APickUpActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EWHEEL_API UObjectSpawner : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UObjectSpawner();

	// Validates if the 
	void CheckAndSpawnObjectsOnNewestTiles(TArray<TileDetails*>* TileLog);
	
	// Removes the objects that belonged to a row that has been removed
	void CheckAndRemoveObjects();

private:
	TArray<AObstacleActor*> mObstacles;
	TArray<APickUpActor*> mPickUps;
};
