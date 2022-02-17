// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshSplineMaster.h"
#include "EWheel/Spline/SplineTilePicker.h"
#include "EWheel/Spline/MeshSplineActor.h"
#include "EWheel/Spline/ObjectSpawner.h"

AMeshSplineMaster::AMeshSplineMaster()
{
	mTilePicker = new SplineTilePicker;
	mTilePicker->SetNumRowsToLog(2);
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
	SetMasterSpline(FMath::Floor((numSplines / 2) % 2));
	AddSplines(numSplines);
}

void AMeshSplineMaster::AttatchSpline(int index)
{
}

void AMeshSplineMaster::DetatchSpline(int index)
{
}

void AMeshSplineMaster::AddSplines(int num)
{
	mSplines.Init(nullptr, num);
	for (int i = 0; i < num; i++)
	{
		mSplines[i] = AddSpline(i, false);
	}
}

// FMath::Floor((mSplines.Num() + 1) / 2)
// i = 0 -> 0
// i = 1 -> 1
// i = 2 -> 1
// i = 3 -> 2

// ((2 * (i % 2)) - 1)
// i = 0 -> -1
// i = 1 -> 1
// i = 2 -> -1

// ((-2 * (i % 2)) + 1)
// i = 0 -> 1
// i = 1 -> -1
// i = 2 -> 1

// Initial offset is half if inmesh num is even, normal if odd (offset = tileoffset/2 if num = even, offset = tileoffset if num = odd)
//float offset = tileOffset * (0.5f * (((inMesh.Num() % 2) + 2) - 1));
// Let origo be the centre of the mesh

// num = 5
// 2 * 150 = 300

// 1 - 1 = 0 * offset
// 300 - 0 = 300;
//offset = FMath::Floor(inMesh.Num() * 0.5f) * 150 - (offset * (1 - (inMesh.Num() % 2))); 

// -300 
AMeshSplineActor* AMeshSplineMaster::AddSpline(int index, bool bEmplaceToArray)
{
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;
	//int numSplines = mSplines.Num();
	//
	//// Initial offset is half if inmesh num is even, normal if odd (offset = tileoffset/2 if num = even, offset = tileoffset if num = odd)
	//float offset = mTileSize * (0.5f * (((numSplines % 2) + 2) - 1));
	//// Let origo be the centre of the mesh
	//offset = FMath::Floor(numSplines * 0.5f) * 150 - (offset * (1 - (numSplines % 2)));
	//
	FVector spawnVector{0.f,0.f,0.f};// { 0.f, -offset + (mTileSize * index), 0.f };

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
	// Semi randomly pick out new tiles and their rottations
	TArray<UStaticMesh*> newTiles = mTilePicker->GetNewTiles(mSplines.Num());
	TArray<int> newTilesRot = mTilePicker->GetLastRowRotation(mSplines.Num());

	// Spawn new spline points in the same location for all splines, but add individual offsets for each spline tile
	// Remove oldest point if the splines will exceed the max point count. 
	if (mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 1 > mMaxNumSplinePoints)
	{
		for (int i = 0; i < mSplines.Num(); i++)
		{
			mSplines[i]->AddSplinePointAndMesh(location, newTiles[i], newTilesRot[i], GetDefaultSplineOffset(i));
			RemoveFirstSplinePointAndMesh(i);
		}
	}
	else
	{
		for (int i = 0; i < mSplines.Num(); i++)
		{
			mSplines[i]->AddSplinePointAndMesh(location, newTiles[i], newTilesRot[i], GetDefaultSplineOffset(i));
		}
	}

	// Spawn obstacles if at least two rows have spawned
	if (mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 3 >= mTilePicker->GetTileLog()->Num())
	{
		TArray<FVector> tileLocations;
		TArray<FRotator> tileRotations;
		int splineIndex = mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 2;
		FVector startPoint = mSplines[mMasterSplineIndex]->GetSpline()->GetLocationAtSplinePoint(splineIndex, ESplineCoordinateSpace::World);
		FVector endPoint = mSplines[mMasterSplineIndex]->GetSpline()->GetLocationAtSplinePoint(splineIndex + 1, ESplineCoordinateSpace::World);
		FVector rightVector = mSplines[mMasterSplineIndex]->GetSpline()->GetRightVectorAtSplinePoint(splineIndex, ESplineCoordinateSpace::World);
		FRotator rotation = mSplines[mMasterSplineIndex]->GetSpline()->GetRotationAtSplinePoint(splineIndex, ESplineCoordinateSpace::World);
		for (int i = 0; i < mSplines.Num(); i++)
		{
			tileLocations.Emplace(startPoint + 0.5f * (endPoint - startPoint) + rightVector * GetDefaultSplineOffset(i));
			tileRotations.Emplace(rotation);
		}

		mObjectSpawner->CheckAndSpawnObjectsOnNewestTiles(mTilePicker->GetTileLog(), &tileLocations, &tileRotations);
		mObjectSpawner->CheckAndRemoveObjectsFromLastRow();
	}
}

void AMeshSplineMaster::SpawnObjectsLastRow()
{
	//mObjectSpawner
	// Check object spawn for each tile
	//int numObstaclesSpawned = 0;
	//for (int i = 0; i < mSplines.Num(); i++)
	//{
	//	// Spawn a point object
	//	if (mObjectSpawner->ValidateSpawnOnTile() FMath::RandRange(0, 99) < mPointSpawnChance)
	//	{
	//		FVector tileCentre = mSplines
	//		SpawnPointObject(tileCentre);
	//	}
	//	// Spawn Obstacle Object
	//	else if (numObstaclesSpawned < mSplines.Num() - 1 && FMath::RandRange(0, 99) < mObstacleSpawnChance)
	//	{
	//		FVector tileCentre = GetTileCentreLastRow(i);
	//		SpawnObstacleObject(tileCentre);
	//		numObstaclesSpawned++;
	//	}
	//}
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
	return GetNumSplinePoints() < mMaxNumSplinePoints;
}

float AMeshSplineMaster::GetDefaultSplineOffset(int splineIndex)
{
	return -mSplines.Num() * mTileSize * 0.5f + (mTileSize * 0.5f) + mTileSize * splineIndex;
}

FVector AMeshSplineMaster::GetLocationAtSplinePoint(int pointIndex)
{
	return mSplines[mMasterSplineIndex]->GetSpline()->GetLocationAtSplinePoint(pointIndex, ESplineCoordinateSpace::World);
}