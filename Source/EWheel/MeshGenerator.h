// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PointLibrary.h"
//Missing .generated here...

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
	UStaticMesh* GenerateStaticMeshFromTile();

private:
	PointLibrary pLib;
};
