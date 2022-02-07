// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include "ProceduralMeshComponent.h" // To include this i had to: add it to .Build.cs then generate files using .Uproject
#include "Engine/StaticMesh.h"

#include "GameFramework/Actor.h"
#include "MeshDescription.h"
#include "ProceduralMeshConversion.h"

#include "KismetProceduralMeshLibrary.h"

MeshGenerator::MeshGenerator()
{
}

MeshGenerator::~MeshGenerator()
{
}

UProceduralMeshComponent* MeshGenerator::GenerateMeshFromTile(int MESH_ENUM)
{
	//FProceduralMeshComponentDetails::ClickedOnConvertToStaticMesh
	UProceduralMeshComponent* generatedMesh = NewObject<UProceduralMeshComponent>();
	generatedMesh->CreateMeshSection(0, *pLib.GetVertices(), *pLib.GetTriangles(), *pLib.GetNormals(), *pLib.GetUVs(), 
									TArray<FColor>(), TArray<FProcMeshTangent>(), true);

	UE_LOG(LogTemp, Warning, TEXT("Vertices: %i"), pLib.GetVertices()->Num());
	UE_LOG(LogTemp, Warning, TEXT("Triangles: %i"), pLib.GetTriangles()->Num());
	UE_LOG(LogTemp, Warning, TEXT("Normals: %i"), pLib.GetNormals()->Num());
	UE_LOG(LogTemp, Warning, TEXT("UVs: %i"), pLib.GetUVs()->Num());

	UE_LOG(LogTemp, Warning, TEXT("Stage 1 complete"));

	return generatedMesh;
}

UStaticMesh* MeshGenerator::GenerateStaticMeshFromTile(TArray<FString>& meshPaths)
{
	UProceduralMeshComponent* ProcMeshComp = NewObject<UProceduralMeshComponent>();
	UStaticMesh* staticMeshAsset;

	for (int i = 0; i < meshPaths.Num(); i++)
	{
		// Import the mesh
		UObject* MeshAsset = StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *meshPaths[i]);
		if (!MeshAsset) return nullptr;
		staticMeshAsset = Cast<UStaticMesh>(MeshAsset);

		// Arrays to store the mesh data
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UVs;
		TArray<FProcMeshTangent> Tangents;

		// Copy the mesh data into the arrays
		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(staticMeshAsset, 0, 0, Vertices, Triangles, Normals, UVs, Tangents);
		
		if (i != 0)
		{ // Move the vertices of the secondary tiles to the side of the center tile.  
			for (int ii = 0; ii < Vertices.Num(); ii++)
			{	// Tileoffset is increased for every 2nd tile placed after the inital one
				// Tileoffset is also alternates between positive and negative for every tile placed (right first, then left)
				Vertices[ii] += FVector{ 0.f, tileOffset * FMath::Floor((i+1)/2), 0.f } * ((2 * (i % 2)) - 1);
			}
		}

		// Create a section using the data copied from the original mesh
		ProcMeshComp->CreateMeshSection(i, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);
	}

	ProcMeshComp->bUseComplexAsSimpleCollision = false;

	//The code below is copied from the "FProceduralMeshComponentDetails::ClickedOnConvertToStaticMesh"
	//function, and trimmed to exclude any unnecessary unreal backend editor configurations. 

	// Find first selected ProcMeshComp
	if(ProcMeshComp)
	{
		FMeshDescription MeshDescription = BuildMeshDescription(ProcMeshComp);

		// If we got some valid data.
		if (MeshDescription.Polygons().Num() > 0)
		{
			// Create StaticMesh object
			UStaticMesh* StaticMesh = NewObject<UStaticMesh>();

			StaticMesh->SetLightingGuid();

			// Add source to new StaticMesh
			
			//int32 LodModelIndex = StaticMesh->GetSourceModel().AddDefaulted();
			//FStaticMeshSourceModel& NewSourceModel = StaticMesh->GetSourceModel(LodModelIndex);
			//NewSourceModel.StaticMeshOwner = this;

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

			//// SIMPLE COLLISION
			if (!ProcMeshComp->bUseComplexAsSimpleCollision)
			{
				StaticMesh->CreateBodySetup();
				UBodySetup* NewBodySetup = StaticMesh->GetBodySetup();
				NewBodySetup->BodySetupGuid = FGuid::NewGuid();
				NewBodySetup->AggGeom.ConvexElems = ProcMeshComp->ProcMeshBodySetup->AggGeom.ConvexElems;
				NewBodySetup->bGenerateMirroredCollision = false;
				NewBodySetup->bDoubleSidedGeometry = true;
				NewBodySetup->CollisionTraceFlag = CTF_UseDefault;
				NewBodySetup->CreatePhysicsMeshes();
			}

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

UStaticMesh* MeshGenerator::StitchStaticMesh(TArray<UStaticMesh*> inMesh)
{
	return nullptr;
}
