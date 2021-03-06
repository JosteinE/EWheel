// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshSplineMaster.h"
#include "EWheel/Spline/SplineTilePicker.h"
#include "EWheel/Spline/MeshSplineActor.h"
#include "EWheel/Spline/ObjectSpawner.h"

AMeshSplineMaster::AMeshSplineMaster()
{
	mTilePicker = new SplineTilePicker;
	mTilePicker->SetNumRowsToLog(2);
	mTilePicker->bAddEdges = bAddEdges;
	mObjectSpawner = CreateDefaultSubobject<UObjectSpawner>(TEXT("ObjectSpawner"));
}

AMeshSplineMaster::~AMeshSplineMaster()
{
	delete mTilePicker;
	mTilePicker = nullptr;
}

void AMeshSplineMaster::SetMaxNumSplinePoints(int maxNum)
{
	mMaxNumSplinePoints = maxNum;
	mObjectSpawner->SetMaxRows(maxNum);
}

void AMeshSplineMaster::ConstructSplines(int numSplines)
{
	if (numSplines < 1) return;
	SetMasterSpline(FMath::Floor(((numSplines + bAddEdges * 2) / 2) % 2));
	AddSplines(numSplines + bAddEdges * 2);
}

void AMeshSplineMaster::AttatchSpline(int index)
{
	//
}

void AMeshSplineMaster::DetatchSpline(int index)
{
	//
}

void AMeshSplineMaster::AddSplines(int num)
{
	mSplines.Init(nullptr, num);
	for (int i = 0; i < num; i++)
	{
		mSplines[i] = AddSpline(i, false);
	}
}

AMeshSplineActor* AMeshSplineMaster::AddSpline(int index, bool bEmplaceToArray)
{
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;

	FVector spawnVector{0.f,0.f,0.f};

	AMeshSplineActor* newSpline = GetWorld()->SpawnActor<AMeshSplineActor>(AMeshSplineActor::StaticClass(), spawnVector, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	newSpline->SetDefaultMaterial(DefaultMaterial);
	
	if(bEmplaceToArray)
		mSplines.Emplace(newSpline);

	return newSpline;
}

void AMeshSplineMaster::RemoveSplines(int num)
{
	int splineNum = mSplines.Num();
	if (num > splineNum)
		num = splineNum;

	for (int i = 0; i < num; i++)
	{
		RemoveSpline();
	}
}

void AMeshSplineMaster::RemoveSpline()
{
	delete mSplines[mSplines.Num() - 1];
	mSplines.RemoveAt(mSplines.Num() - 1);
}

FVector AMeshSplineMaster::GenerateNewPointLocation()
{
	FVector LastSplinePointDirection = mSplines[mMasterSplineIndex]->GetSpline()->GetDirectionAtSplinePoint(mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	FVector LastSplinePointRightVector = mSplines[mMasterSplineIndex]->GetSpline()->GetRightVectorAtSplinePoint(mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);

	float rightAmount = FMath::RandRange(-mTileSize / mSplines.Num() * 0.5f, mTileSize / mSplines.Num() * 0.5f);
	LastSplinePointDirection.Z += FMath::RandRange(-mSplineVerticalStep, mSplineVerticalStep);

	FVector newLocation = mSplines[mMasterSplineIndex]->GetSpline()->GetWorldLocationAtSplinePoint(mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 1) + LastSplinePointDirection * FVector{ mTileSize, mTileSize, 1.f } + LastSplinePointRightVector * rightAmount;
	newLocation.Z = FMath::Clamp(newLocation.Z, mSplineVerticalMin, mSplineVerticalMax);

	return newLocation;
}

void AMeshSplineMaster::AddPoint(FVector location)
{
	// Semi randomly pick out new tiles and their rotations
	TArray<UStaticMesh*> newTiles = mTilePicker->GetNewTiles(mSplines.Num() - bAddEdges * 2);
	TArray<int> newTilesRot = mTilePicker->GetLastRowRotation(mSplines.Num() - bAddEdges * 2);

	// Spawn new spline points in the same location for all splines, but add individual offsets for each spline tile
	// Remove oldest point if the splines will exceed the max point count. 
	if (mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 1 > mMaxNumSplinePoints)
	{
		if (bAddEdges)
		{
			RemoveFirstSplinePointAndMesh(0);
			RemoveFirstSplinePointAndMesh(mSplines.Num() - 1);
		}

		for (int i = bAddEdges; i < mSplines.Num() - bAddEdges; i++)
		{
			RemoveFirstSplinePointAndMesh(i);
			mSplines[i]->AddSplinePointAndMesh(location, newTiles[i], newTilesRot[i - bAddEdges], GetDefaultSplineOffset(i));
		}
	}
	else
	{
		for (int i = bAddEdges; i < mSplines.Num() - bAddEdges; i++)
		{
			mSplines[i]->AddSplinePointAndMesh(location, newTiles[i], newTilesRot[i - bAddEdges], GetDefaultSplineOffset(i));
		}
	}

	if (bAddEdges)
	{
		mSplines[0]->AddSplinePointAndMesh(location, newTiles[0], 0, GetDefaultSplineOffset(0));
		mSplines[mSplines.Num() - 1]->AddSplinePointAndMesh(location, newTiles[mSplines.Num() - 1], 0, GetDefaultSplineOffset(mSplines.Num() - 1));
	}

	// Spawn obstacles if at least two rows of tiles have spawned
	if (bSpawnObjects && mSplines.Num() > 1 && mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 3 >= mTilePicker->GetTileLog()->Num() / mSplines.Num())
		SpawnObjectsLastRow();
}

void AMeshSplineMaster::SpawnObjectsLastRow()
{
	TArray<FVector> tileLocations;
	int splineIndex = mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 3;

	FVector location = mSplines[mMasterSplineIndex]->GetSpline()->GetLocationAtSplineInputKey(splineIndex + 0.5f, ESplineCoordinateSpace::World);
	FRotator rotation = mSplines[mMasterSplineIndex]->GetSpline()->GetRotationAtSplineInputKey(splineIndex + 0.5f, ESplineCoordinateSpace::World);
	FVector rightVector = mSplines[mMasterSplineIndex]->GetSpline()->GetRightVectorAtSplineInputKey(splineIndex + 0.5f, ESplineCoordinateSpace::World);

	for (int i = bAddEdges; i < mSplines.Num() - bAddEdges; i++)
	{
		//tileLocations.Emplace(startPoint + 0.5f * (endPoint - startPoint) + rightVector * GetDefaultSplineOffset(i));
		tileLocations.Emplace(location + rightVector * GetDefaultSplineOffset(i));
	}

	//if (bAddEdges)
	//{
	//	TArray<TileDetails*> tLog = mTilePicker->GetEdgeLessTileLog();
	//	mObjectSpawner->CheckAndSpawnObjectsOnNewestTiles(&tLog, tileLocations, rotation);
	//}
	//else
	mObjectSpawner->CheckAndSpawnObjectsOnNewestTiles(mTilePicker->GetTileLog(), tileLocations, rotation);

	mObjectSpawner->CheckAndRemoveObjectsFromLastRow();
}

void AMeshSplineMaster::RemoveFirstSplinePointAndMesh(int splineIndex)
{
	mSplines[splineIndex]->RemoveFirstSplinePointAndMesh(true);
}

int AMeshSplineMaster::GetNumSplinePoints()
{
	return mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints();
}

bool AMeshSplineMaster::GetIsAtMaxSplinePoints()
{
	return GetNumSplinePoints() >= mMaxNumSplinePoints + 1;
}

float AMeshSplineMaster::GetDefaultSplineOffset(int splineIndex)
{
	return -mSplines.Num() * mTileSize * 0.5f + (mTileSize * 0.5f) + mTileSize * splineIndex;
}

void AMeshSplineMaster::SetObstacleSpawnChance(int obstacleSpawnChance)
{
	mObjectSpawner->SetObstacleSpawnChance(obstacleSpawnChance);
}

void AMeshSplineMaster::SetPointSpawnChance(int pointSpawnChance)
{
	mObjectSpawner->SetPointSpawnChance(pointSpawnChance);
}

void AMeshSplineMaster::SetPowerUpSpawnChance(int powerUpSpawnChance)
{
	mObjectSpawner->SetPowerUpSpawnChance(powerUpSpawnChance);
}

void AMeshSplineMaster::SetSpawnPits(bool spawnPits)
{
	mTilePicker->SetSpawnPits(spawnPits);
}

void AMeshSplineMaster::SetSpawnRamps(bool spawnRamps)
{
	mTilePicker->SetSpawnRamps(spawnRamps);
}

void AMeshSplineMaster::SetSpawnHoles(bool spawnHoles)
{
	mTilePicker->SetSpawnHoles(spawnHoles);
}

void AMeshSplineMaster::SetUseHighResModels(bool highResModels)
{
	mTilePicker->SetUseHighResModels(highResModels);
}

void AMeshSplineMaster::LoadFromJson(TSharedPtr<FJsonObject> inJson)
{
	SetSpawnPits(inJson->GetBoolField("PitsEnabled"));
	SetSpawnRamps(inJson->GetBoolField("RampsEnabled"));
	SetSpawnHoles(inJson->GetBoolField("HolesEnabled")); // Should only be enabled once jump is acquired
	SetObstacleSpawnChance(inJson->GetIntegerField("ObstacleSpawnChance"));
	SetPointSpawnChance(inJson->GetIntegerField("PointSpawnChance"));
	SetPowerUpSpawnChance(inJson->GetIntegerField("PowerupSpawnChance"));

	mTilePicker->LoadFromJson(inJson->GetObjectField("TilePicker"));
}

FVector AMeshSplineMaster::GetLocationAtSplinePoint(int pointIndex)
{
	return mSplines[mMasterSplineIndex]->GetSpline()->GetLocationAtSplinePoint(pointIndex, ESplineCoordinateSpace::World);
}

FVector AMeshSplineMaster::GetLocationAtSplineInputKey(float inputKey)
{
	return mSplines[mMasterSplineIndex]->GetSpline()->GetLocationAtSplineInputKey(inputKey, ESplineCoordinateSpace::World);
}

void AMeshSplineMaster::GetLocationAndRotationAtSplineInputKey(FVector& returnLocation, FRotator& returnRotation, float inputKey)
{
	returnLocation = mSplines[mMasterSplineIndex]->GetSpline()->GetLocationAtSplineInputKey(inputKey, ESplineCoordinateSpace::World);
	returnRotation = mSplines[mMasterSplineIndex]->GetSpline()->GetRotationAtSplineInputKey(inputKey, ESplineCoordinateSpace::World);
}

void AMeshSplineMaster::GetLocationAndRotationAtSplinePoint(FVector& returnLocation, FRotator& returnRotation, int pointIndex)
{
	returnLocation = mSplines[mMasterSplineIndex]->GetSpline()->GetLocationAtSplinePoint(pointIndex, ESplineCoordinateSpace::World);
	returnRotation = mSplines[mMasterSplineIndex]->GetSpline()->GetRotationAtSplinePoint(pointIndex, ESplineCoordinateSpace::World);
}

float AMeshSplineMaster::FindInputKeyClosestToWorldLocation(FVector& location)
{
	return mSplines[mMasterSplineIndex]->GetSpline()->FindInputKeyClosestToWorldLocation(location);
}
