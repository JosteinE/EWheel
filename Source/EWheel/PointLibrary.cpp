//// Fill out your copyright notice in the Description page of Project Settings.
//
//#include "PointLibrary.h"
//#include "UObject/ConstructorHelpers.h"
//#include "Engine/StaticMesh.h"
//
//PointLibrary::PointLibrary()
//{
//	AddMeshToLibrary("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'");
//}
//
//PointLibrary::~PointLibrary()
//{
//}
//
//void PointLibrary::AddMeshToLibrary(FString fileRef)
//{
//	UObject* MeshAsset = StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *fileRef);
//	if (!MeshAsset) return;
//
//	UStaticMesh* StaticMesh = Cast<UStaticMesh>(MeshAsset);
//
//	if (StaticMesh->GetRenderData()->LODResources.Num() > 0)
//	{
//		FStaticMeshVertexBuffers* meshVertexBuffer = &StaticMesh->GetRenderData()->LODResources[0].VertexBuffers;
//		unsigned int numVertices = meshVertexBuffer->PositionVertexBuffer.GetNumVertices();
//
//		for (unsigned int i = 0; i < numVertices; i++)
//		{
//			StoredVertices.Emplace(meshVertexBuffer->PositionVertexBuffer.VertexPosition(i));
//			//FVector4 verTan = meshVertexBuffer->StaticMeshVertexBuffer.VertexTangentX(i);
//			StoredNormals.Emplace(FVector{ meshVertexBuffer->StaticMeshVertexBuffer.VertexTangentX(i) });
//			StoredUVs.Emplace(FVector2D{ meshVertexBuffer->PositionVertexBuffer.VertexPosition(i) });
//		}
//
//		// Assumes the vertices are placed in the order of their triangles. Every 4th point is the start of a new triangle
//		for (unsigned int i = 0; i < numVertices; i += 3)
//		{
//			StoredTriangles.Emplace(i);
//		}
//	}
//}
//
//TArray<FVector>* PointLibrary::GetVertices()
//{
//	return &StoredVertices;
//}
//
//TArray<int>* PointLibrary::GetTriangles()
//{
//	return &StoredTriangles;
//}
//
//TArray<FVector>* PointLibrary::GetNormals()
//{
//	return &StoredNormals;
//}
//
//TArray<FVector2D>* PointLibrary::GetUVs()
//{
//	return &StoredUVs;
//}
