// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshSplineMaster.h"
#include "EWheel/Spline/SplineTilePicker.h"
#include "EWheel/Spline/MeshSplineActor.h"

AMeshSplineMaster::AMeshSplineMaster()
{
	TilePicker = new SplineTilePicker;
	TilePicker->SetNumRowsToLog(2);
}

AMeshSplineMaster::~AMeshSplineMaster()
{
	delete TilePicker;
	TilePicker = nullptr;
}

void AMeshSplineMaster::ConstructSplines(int numSplines)
{
	if (numSplines <= 0) return;

	AddSpline(numSplines);
}

void AMeshSplineMaster::AttatchSpline(int index)
{
}

void AMeshSplineMaster::DetatchSpline(int index)
{
}

void AMeshSplineMaster::AddSpline(int num)
{
	for (int i = 0; i < num; i++)
	{
		AddSpline();
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
AMeshSplineActor* AMeshSplineMaster::AddSpline()
{
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;
	int numSplines = mSplines.Num();
	float spawnOffset = mSplineOffset * FMath::Floor((numSplines + 1) / 2) * ((-2 * (numSplines % 2)) + 1);
	FVector spawnVector{ 0.f, 0.f, 0.f };
	if (numSplines > 0)
		spawnVector = mSplines[mMasterSplineIndex]->GetActorLocation() * spawnOffset;

	AMeshSplineActor* newSpline = GetWorld()->SpawnActor<AMeshSplineActor>(AMeshSplineActor::StaticClass(), spawnVector, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	newSpline->SetDefaultMaterial(DefaultMaterial);
	mSplines.Emplace(newSpline);
	return newSpline;
}

void AMeshSplineMaster::RemoveSpline(int num)
{
	int splineNum = mSplines.Num();
	if (num > splineNum)
		num = splineNum;

	// 10
	// 1
	for (int splineIndex = splineNum - 1; splineIndex > splineNum - 1 - num; splineIndex--)
	{
		AMeshSplineActor* spline = mSplines[splineIndex];
		delete spline;
	}
}

void AMeshSplineMaster::AddPoint(FVector location)
{
}

void AMeshSplineMaster::RemoveFirstSplinePointsAndMeshes()
{
}

void AMeshSplineMaster::AssignMesh(int splineIndex)
{
}

void AMeshSplineMaster::GetNewTiles()
{
}
