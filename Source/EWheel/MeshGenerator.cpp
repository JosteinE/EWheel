// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include "ProceduralMeshComponent.h" // To include this i had to: add it to .Build.cs then generate files using .Uproject
#include "Engine/StaticMesh.h"

#include "GameFramework/Actor.h"
#include "Engine/StaticMesh.h"
#include "MeshDescription.h"
#include "ProceduralMeshConversion.h"

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
	generatedMesh->RegisterComponent();
	return generatedMesh;
}

UStaticMesh* MeshGenerator::GenerateStaticMeshFromTile()
{
	UProceduralMeshComponent* ProcMeshComp = GenerateMeshFromTile(0);

	// Find first selected ProcMeshComp
	if (ProcMeshComp != nullptr)
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

			////// MATERIALS
			//TSet<UMaterialInterface*> UniqueMaterials;
			//const int32 NumSections = ProcMeshComp->GetNumSections();
			//for (int32 SectionIdx = 0; SectionIdx < NumSections; SectionIdx++)
			//{
			//	FProcMeshSection* ProcSection =
			//		ProcMeshComp->GetProcMeshSection(SectionIdx);
			//	UMaterialInterface* Material = ProcMeshComp->GetMaterial(SectionIdx);
			//	UniqueMaterials.Add(Material);
			//}
			//// Copy materials to new mesh
			//for (auto* Material : UniqueMaterials)
			//{
			//	StaticMesh->GetStaticMaterials().Add(FStaticMaterial(Material));
			//}

			////Set the Imported version before calling the build
			//StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;

			//// Build mesh from source
			//StaticMesh->Build(false);
			//StaticMesh->PostEditChange();

			return StaticMesh;
		}
	}
	return nullptr;
}
