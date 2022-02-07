// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class UProceduralMeshComponent;
class UStaticMesh;

class EWHEEL_API MeshGenerator
{
public:
	MeshGenerator();
	~MeshGenerator();

	UProceduralMeshComponent* GenerateMeshFromTile(int MESH_ENUM);
	UStaticMesh* GenerateStaticMeshFromTile(TArray<FString>& meshPaths);
	UStaticMesh* StitchStaticMesh(TArray<UStaticMesh*> inMesh);


private:
	float tileOffset = 150.f;
};
