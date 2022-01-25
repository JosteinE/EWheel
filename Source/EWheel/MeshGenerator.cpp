// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshGenerator.h"
#include "ProceduralMeshComponent.h" // To include this i had to: add it to .Build.cs then generate files using .Uproject
#include "Engine/StaticMesh.h"

#include "GameFramework/Actor.h"
#include "Engine/StaticMesh.h"
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
	//UObject* MeshAsset = StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'"));
	//if (!MeshAsset) return nullptr;

	//UStaticMesh* staticMeshAsset = Cast<UStaticMesh>(MeshAsset);
	//UStaticMeshComponent* meshComp = NewObject<UStaticMeshComponent>();
	//meshComp->SetStaticMesh(staticMeshAsset);

	UProceduralMeshComponent* ProcMeshComp = NewObject<UProceduralMeshComponent>();
	UStaticMesh* staticMeshAsset;
	//UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(meshComp, 0, ProcMeshComp, true);

	for (int i = 0; i < meshPaths.Num(); i++)
	{
		UObject* MeshAsset = StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *meshPaths[i]);
		if (!MeshAsset) return nullptr;
		staticMeshAsset = Cast<UStaticMesh>(MeshAsset);

		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UVs;
		TArray<FProcMeshTangent> Tangents;

		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(staticMeshAsset, 0, 0, Vertices, Triangles, Normals, UVs, Tangents);
		
		if (i != 0)
		{ // Move the vertices of t
			for (int ii = 0; ii < Vertices.Num(); ii++)
			{
				Vertices[ii] += FVector{ 0.f, 150.f, 0.f } * ((2 * (i % 2)) - 1);
			}
		}


		ProcMeshComp->CreateMeshSection(i, Vertices, Triangles, Normals, UVs, TArray<FColor>(), Tangents, true);

		//ProcMeshComp->Bounds.Origin = FVector{ 0, 150, 0 } * ((2 * (i % 2)) - 1);


		//ProcMeshComp->GetProcMeshSection(i)->SectionLocalBox(FVector{ 0, 150, 0 } *((2 * (i % 2)) - 1));
		//ProcMeshComp->GetProcMeshSection(i)->bEnableCollision = false;
		//ProcMeshComp->SetRelativeLocation_Direct(FVector{ 0, 150, 0 } * ((2 * (i % 2)) - 1));
		UE_LOG(LogTemp, Warning, TEXT("ProcLoc: %f, %f, %f"), ProcMeshComp->GetComponentLocation().X, ProcMeshComp->GetComponentLocation().Y, ProcMeshComp->GetComponentLocation().Z);
	}
	ProcMeshComp->CalcLocalBounds();

	//UProceduralMeshComponent* ProcMeshComp = GenerateMeshFromTile(0);

	//// Find first selected ProcMeshComp
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

			//// Build mesh from source
			StaticMesh->Build(false);
			StaticMesh->PostEditChange();

			return StaticMesh;
		}
	}
	return nullptr;
}
