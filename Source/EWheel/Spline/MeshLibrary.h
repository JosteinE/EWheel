// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum MeshType
{
	DEFAULT,
	PIT,
	PIT_4W,
	PIT_END_SN,
	PIT_EX,
	PIT_L,
	PIT_T,
	RAMP,
	RAMP_L,
	RAMP_M,
	RAMP_R,
	HOLE,
	HOLE_4W,
	HOLE_END_SN,
	HOLE_EX,
	HOLE_L,
	HOLE_T
};

enum EdgeMeshType
{
	EDGE_DIVE_L,
	EDGE_DIVE_R,
	EDGE_HILL_L,
	EDGE_HILL_R,
	EDGE_SLOPE_L,
	EDGE_SLOPE_R,
	EDGE_HOLE_L,
	EDGE_HOLE_R
};

enum MeshCategories
{
	CATEGORY_DEFAULT,
	CATEGORY_PIT,
	CATEGORY_RAMP,
	CATEGORY_HOLE,
	CATEGORY_EDGE
};

class UStaticMesh;

/**
 * 
 */
class EWHEEL_API MeshLibrary
{
public:
	MeshLibrary();
	~MeshLibrary();

	UStaticMesh* GetMesh(int meshType);
	UStaticMesh* GetEdgeMesh(int edgeMeshType);
private:
	TArray<UStaticMesh*> mLibrary;
	TArray<UStaticMesh*> mEdgeMeshLibrary;
};
