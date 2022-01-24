// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//Missing .generated here...

/**
 * 
 */
class UProceduralMeshComponent;

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

class EWHEEL_API MeshGenerator : public AActor
{
public:
	MeshGenerator();
	~MeshGenerator();

	UProceduralMeshComponent* GenerateMeshFromTile(int MESH_ENUM);
};
