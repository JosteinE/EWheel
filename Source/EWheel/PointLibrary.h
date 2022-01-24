// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Rendering/PositionVertexBuffer.h"
#include "CoreMinimal.h"

/**
 * 
 */
enum class MESH
{
	DEFAULT,
	PIT,
	PIT4W,
	PIT_END_SN,
	PIT_EX,
	PIT_L,
	PIT_T,
	PIT_RAMP,
	PIT_RAMP_L,
	PIT_RAMP_M,
	PIT_RAMP_R
};

class EWHEEL_API PointLibrary
{
public:
	PointLibrary();
	~PointLibrary();

	void AddMeshToLibrary(FString fileRef);
	TArray<FVector>* GetVertices();
	TArray<int>* GetTriangles();
	TArray<FVector>* GetNormals();
	TArray<FVector2D>* GetUVs();

private:
	TArray<FVector> StoredVertices;
	TArray<int> StoredTriangles;
	TArray<FVector> StoredNormals;
	TArray<FVector2D> StoredUVs;
};
