// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Rendering/PositionVertexBuffer.h"
#include "CoreMinimal.h"

/**
 * 
 */
class EWHEEL_API PointLibrary
{
public:
	PointLibrary();
	~PointLibrary();

	void AddMeshToLibrary(FString fileRef);
	TArray<FVector>* GetStoredMesh();
private:
	TArray<FVector> StoredVertices;
};
