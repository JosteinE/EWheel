// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshSplineMaster.h"
#include "EWheel/Spline/SplineTilePicker.h"
#include "EWheel/Spline/MeshSplineActor.h"

AMeshSplineMaster::AMeshSplineMaster()
{
	mTilePicker = new SplineTilePicker;
	mTilePicker->SetNumRowsToLog(2);
}

AMeshSplineMaster::~AMeshSplineMaster()
{
	delete mTilePicker;
	mTilePicker = nullptr;
}

void AMeshSplineMaster::ConstructSplines(int numSplines)
{
	if (numSplines <= 0) return;

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
	SetMasterSpline(FMath::Floor(num * 0.5f));
	mSplines.Init(nullptr, num);
	for (int i = 0; i < num; i++)
	{
		mSplines[i] = AddSpline(false);
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
AMeshSplineActor* AMeshSplineMaster::AddSpline(bool bEmplaceToArray)
{
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;
	int numSplines = mSplines.Num();
	
	// Initial offset is half if inmesh num is even, normal if odd (offset = tileoffset/2 if num = even, offset = tileoffset if num = odd)
	float offset = mSplineOffset * (0.5f * (((numSplines % 2) + 2) - 1));
	// Let origo be the centre of the mesh
	offset = FMath::Floor(numSplines * 0.5f) * 150 - (offset * (1 - (numSplines % 2)));
	
	FVector spawnVector{ 0.f, -offset + (mSplineOffset * numSplines), 0.f };

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

void AMeshSplineMaster::AddPoint(FVector location)
{
	TArray<UStaticMesh*> newTiles = mTilePicker->GetNewTiles(mSplines.Num());
	TArray<int> newTilesRot = mTilePicker->GetLastRowRotation(mSplines.Num());
	
	if (mSplines[mMasterSplineIndex]->GetSpline()->GetNumberOfSplinePoints() - 1 > mMaxNumSplinePoints)
	{
		for (int i = 0; i < mSplines.Num(); i++)
		{
			mSplines[i]->AddSplinePointAndMesh(location);
			//mSplines[i]->AssignMesh(newTiles[i])
			RemoveFirstSplinePointAndMesh(i);
		}
	}
	else
	{
		for (int i = 0; i < mSplines.Num(); i++)
		{
			mSplines[i]->AddSplinePointAndMesh(location);
		}
	}
}

void AMeshSplineMaster::RemoveFirstSplinePointAndMesh(int splineIndex)
{
	mSplines[splineIndex]->RemoveFirstSplinePointAndMesh(true);
}
