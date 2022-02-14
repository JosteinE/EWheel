// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include "ProceduralMeshComponent.h" // To include this i had to: add it to .Build.cs then generate files using .Uproject
#include "Engine/StaticMesh.h"

#include "GameFramework/Actor.h"
#include "MeshDescription.h"
#include "ProceduralMeshConversion.h"

#include "KismetProceduralMeshLibrary.h"

#include "StaticMeshDescription.h"

MeshGenerator::MeshGenerator()
{
	TArray<FString> meshPaths;
	// Edge Meshes
	//// Dive
	//meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Dive_B_150x150_Smooth.PathEdge_Dive_B_150x150_Smooth'");
	//meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Dive_F_150x150_Smooth.PathEdge_Dive_F_150x150_Smooth'");
	//meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Dive_L_150x150_Smooth.PathEdge_Dive_L_150x150_Smooth'");
	//meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Dive_R_150x150_Smooth.PathEdge_Dive_R_150x150_Smooth'");
	//// Hill
	//meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Hill_L_150x150_Smooth.PathEdge_Hill_L_150x150_Smooth'");
	//meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Hill_R_150x150_Smooth.PathEdge_Hill_R_150x150_Smooth'");
	//// Slope
	//meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Slope_L_150x150_Smooth.PathEdge_Slope_L_150x150_Smooth'");
	//meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Slope_R_150x150_Smooth.PathEdge_Slope_R_150x150_Smooth'");

	//OLD
	// Dive
	meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Dive_B_150x150.PathEdge_Dive_B_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Dive_F_150x150.PathEdge_Dive_F_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Dive_L_150x150.PathEdge_Dive_L_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Dive_R_150x150.PathEdge_Dive_R_150x150'");
	// Hill
	meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Hill_L_150x150.PathEdge_Hill_L_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Hill_R_150x150.PathEdge_Hill_R_150x150'");
	// Slope
	meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Slope_L_150x150.PathEdge_Slope_L_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Slope_R_150x150.PathEdge_Slope_R_150x150'");

	for (int i = 0; i < meshPaths.Num(); i++)
	{
		UObject* MeshAsset = StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *meshPaths[i]);
		if (MeshAsset)
			EdgeMeshLibrary.Emplace(Cast<UStaticMesh>(MeshAsset));
	}
}

MeshGenerator::~MeshGenerator()
{
}

//UProceduralMeshComponent* MeshGenerator::GenerateMeshFromTile(int MESH_ENUM)
//{
//	////FProceduralMeshComponentDetails::ClickedOnConvertToStaticMesh
//	//UProceduralMeshComponent* generatedMesh = NewObject<UProceduralMeshComponent>();
//	//generatedMesh->CreateMeshSection(0, *pLib.GetVertices(), *pLib.GetTriangles(), *pLib.GetNormals(), *pLib.GetUVs(), 
//	//								TArray<FColor>(), TArray<FProcMeshTangent>(), true);
//
//	//UE_LOG(LogTemp, Warning, TEXT("Vertices: %i"), pLib.GetVertices()->Num());
//	//UE_LOG(LogTemp, Warning, TEXT("Triangles: %i"), pLib.GetTriangles()->Num());
//	//UE_LOG(LogTemp, Warning, TEXT("Normals: %i"), pLib.GetNormals()->Num());
//	//UE_LOG(LogTemp, Warning, TEXT("UVs: %i"), pLib.GetUVs()->Num());
//
//	//UE_LOG(LogTemp, Warning, TEXT("Stage 1 complete"));
//
//	//return generatedMesh;
//}

UStaticMesh* MeshGenerator::GenerateStaticMeshFromTile(TArray<FString>& meshPaths)
{
	TArray<UStaticMesh*> meshAssets;

	for (int i = 0; i < meshPaths.Num(); i++)
	{
		// Import the mesh
		UObject* MeshAsset = StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *meshPaths[i]);
		if (!MeshAsset) return nullptr;
		meshAssets.Emplace(Cast<UStaticMesh>(MeshAsset));
	}

	return StitchStaticMesh(TArray<int>(), meshAssets);
}

UStaticMesh* MeshGenerator::StitchStaticMesh(TArray<int> inRot, TArray<UStaticMesh*> inMesh)
{
	UProceduralMeshComponent* ProcMeshComp = NewObject<UProceduralMeshComponent>();

	if (bAddEdgeMesh)
	{
		inMesh.EmplaceAt(0, EdgeMeshLibrary[3]);
		inRot.EmplaceAt(0, 0);
		inMesh.Emplace(EdgeMeshLibrary[2]);
		inRot.Emplace(0);
	}
	// Pits Test
	//if(FMath::RandRange(0, 99) < 10)
	//{
	//	for (int i = 1; i < inMesh.Num() - 1; i++)
	//	{
	//		inMesh[i] = EdgeMeshLibrary[0];
	//		inRot[i] = 0;
	//	}
	//}

	// */

	// Initial offset is half if inmesh num is even, normal if odd
	float offset = tileOffset * (0.5f * (((inMesh.Num() % 2) + 2) - 1));
	// Let origo be the centre of the mesh
	offset = FMath::Floor(inMesh.Num() * 0.5f) * 150 - (offset * (1 - (inMesh.Num() % 2)));

	for (int i = 0; i < inMesh.Num(); i++)
	{
		// Arrays to store the mesh data
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UVs;
		TArray<FProcMeshTangent> Tangents;

		// Copy the mesh data into the arrays
		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(inMesh[i], 0, 0, Vertices, Triangles, Normals, UVs, Tangents);

		if (inMesh.Num() > 1)
		{
			for (int ii = 0; ii < Vertices.Num(); ii++)
			{
				// Rotate vertices
				Vertices[ii] = FRotator{ 0.f, inRot[i] * 90.f, 0.f }.RotateVector(Vertices[ii]);
				// Rotate normals
				Normals[ii] = FRotator{ 0.f, inRot[i] * 90.f, 0.f }.RotateVector(Normals[ii]);
				// Move vertices
				Vertices[ii] += FVector{ 0.f, -offset + (tileOffset * i), 0.f };
			}
		}

		// Create a section using the data copied from the original mesh
		ProcMeshComp->CreateMeshSection(i, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
	}

	ProcMeshComp->bUseComplexAsSimpleCollision = true;

	//The code below is copied from the "FProceduralMeshComponentDetails::ClickedOnConvertToStaticMesh"
	//function, and trimmed to exclude any unnecessary unreal backend editor configurations. 

	// Find first selected ProcMeshComp
	if (ProcMeshComp)
	{
		FMeshDescription MeshDescription = BuildMeshDescription(ProcMeshComp);

		// If we got some valid data.
		if (MeshDescription.Polygons().Num() > 0)
		{
			// Create StaticMesh object
			UStaticMesh* StaticMesh = NewObject<UStaticMesh>();

			StaticMesh->SetLightingGuid();

			// Add source to new StaticMesh
			FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
			SrcModel.BuildSettings.bRecomputeNormals = false;
			SrcModel.BuildSettings.bRecomputeTangents = false;
			SrcModel.BuildSettings.bRemoveDegenerates = false;
			SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
			SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
			SrcModel.BuildSettings.bGenerateLightmapUVs = true;
			SrcModel.BuildSettings.SrcLightmapIndex = 0;
			SrcModel.BuildSettings.DstLightmapIndex = 1;
			StaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
			StaticMesh->CommitMeshDescription(0);

			////// SIMPLE COLLISION Probably wont need this. 
			//if (!ProcMeshComp->bUseComplexAsSimpleCollision)
			//{
			//	StaticMesh->CreateBodySetup();
			//	UBodySetup* NewBodySetup = StaticMesh->GetBodySetup();
			//	NewBodySetup->BodySetupGuid = FGuid::NewGuid();
			//	NewBodySetup->AggGeom.ConvexElems = ProcMeshComp->ProcMeshBodySetup->AggGeom.ConvexElems;
			//	NewBodySetup->bGenerateMirroredCollision = false;
			//	NewBodySetup->bDoubleSidedGeometry = true;
			//	NewBodySetup->CollisionTraceFlag = CTF_UseDefault;
			//	NewBodySetup->CreatePhysicsMeshes();
			//}

			//// MATERIALS
			TSet<UMaterialInterface*> UniqueMaterials;
			const int32 NumSections = ProcMeshComp->GetNumSections();
			for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
			{
				FProcMeshSection* ProcSection =
					ProcMeshComp->GetProcMeshSection(SectionIdx);
				UMaterialInterface* Material = ProcMeshComp->GetMaterial(SectionIdx);
				UniqueMaterials.Add(Material);
			}
			// Copy materials to new mesh
			for (auto* Material : UniqueMaterials)
			{
				StaticMesh->GetStaticMaterials().Add(FStaticMaterial(Material));
			}

			//Set the Imported version before calling the build
			StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

			// Build mesh from source
			StaticMesh->Build(false);
			StaticMesh->PostEditChange();

			return StaticMesh;
		}
	}
	return nullptr;
}
