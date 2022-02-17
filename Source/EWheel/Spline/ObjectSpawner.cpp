// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/ObjectSpawner.h"
#include "EWheel/Spline/MeshCategoriesAndTypes.h"
#include "EWheel/Spline/TileDetails.h"


// Sets default values for this component's properties
UObjectSpawner::UObjectSpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UObjectSpawner::CheckAndSpawnObjectsOnNewestTiles(TArray<TileDetails*>* TileLog)
{
	TArray<int> possibleTilesIndices;

	// Get current available tiles
	for (int i = FMath::Floor(TileLog->Num() * 0.5f) - 1; i < TileLog->Num(); i++)
	{
		if((*TileLog)[i]->m_MeshCategory != MeshCategories::CATEGORY_HOLE && (*TileLog)[i]->m_MeshCategory != MeshCategories::CATEGORY_RAMP)
		{
			possibleTilesIndices.Emplace(i);
		}
	}


}

void UObjectSpawner::CheckAndRemoveObjects()
{
}
