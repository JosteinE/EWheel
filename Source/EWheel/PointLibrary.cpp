// Fill out your copyright notice in the Description page of Project Settings.

#include "PointLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

PointLibrary::PointLibrary()
{
	AddMeshToLibrary("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'");
}

PointLibrary::~PointLibrary()
{
}

void PointLibrary::AddMeshToLibrary(FString fileRef)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(*fileRef);
	if (MeshAsset.Succeeded())
		MeshAsset.Object;

	if (MeshAsset.Object->RenderData->LODResources.Num() > 0)
	{
		FPositionVertexBuffer* VertexBuffer = &MeshAsset.Object->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;

		for (int i = 0; i < VertexBuffer->GetNumVertices(); i++)
		{
			StoredVertices.Emplace(VertexBuffer->VertexPosition(i));
		}
	}
}

TArray<FVector>* PointLibrary::GetStoredMesh()
{
	return &StoredVertices;
}
