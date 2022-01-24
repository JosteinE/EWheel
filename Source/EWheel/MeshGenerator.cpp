// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include "ProceduralMeshComponent.h" // To include this i had to: add it to .Build.cs then generate files using .Uproject
#include "PointLibrary.h"

MeshGenerator::MeshGenerator()
{
}

MeshGenerator::~MeshGenerator()
{
}

UProceduralMeshComponent* MeshGenerator::GenerateMeshFromTile(int MESH_ENUM)
{
	UProceduralMeshComponent* generatedMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));


	//generatedMesh->CreateMeshSection()

	return nullptr;
}
