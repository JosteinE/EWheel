// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshSplineActor.h"
#include "UObject/ConstructorHelpers.h"
#include "PhysicsEngine/BodySetup.h"

AMeshSplineActor::AMeshSplineActor()
{
	//If there is trouble converting this actor from code to blueprint, comment this out!
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'"));
	if (MeshAsset.Succeeded())
		DefaultMesh = MeshAsset.Object;
}

void AMeshSplineActor::OnConstruction(const FTransform& Transform)
{
	int startConAt = 0;

	// (Re)construct from 0 or from a desired starting point
	if (numMeshToReConPostInit > 0 && numMeshToReConPostInit <= SplineMeshComponent.Num())
		startConAt = SplineMeshComponent.Num() - numMeshToReConPostInit;

	// Construct the spline mesh
	for (int SplineCount = startConAt; SplineCount < GetSpline()->GetNumberOfSplinePoints() - 1; SplineCount++)
	{
		ConstructMesh(SplineCount);
	}
}

void AMeshSplineActor::ConstructMesh(int SplineIndex, int MeshType)
{
	// Ensure that our mesh exists, otherwise return
	UStaticMesh* mesh;
	if (MeshType == 0 && DefaultMesh)
	{
		mesh = DefaultMesh;
	}
	else if (MeshType != 0)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_N_150x150.Ground_Ramp_N_150x150'"));
		if (MeshAsset.Succeeded())
			mesh = MeshAsset.Object;
		else
			return;
	}
	else
		return;

	//Use the previous point as the starting location
	const FVector StartingPoint = GetSpline()->GetLocationAtSplinePoint(SplineIndex, ESplineCoordinateSpace::Local);
	//Get the tangent belonging to our previous point
	const FVector StartTangent = GetSpline()->GetTangentAtSplinePoint(SplineIndex, ESplineCoordinateSpace::Local);
	//Get the next point along our curve
	const FVector EndPoint = GetSpline()->GetLocationAtSplinePoint(SplineIndex + 1, ESplineCoordinateSpace::Local);
	//Get the tangent belonging to our next point
	const FVector EndTangent = GetSpline()->GetTangentAtSplinePoint(SplineIndex + 1, ESplineCoordinateSpace::Local);

	//Set the index account for the number of rows/meshes.
	SplineIndex *= tilesPerRow;
	// Construct our new mesh component
	SplineMeshComponent.Emplace(NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass()));
	// Assign mesh to the new component
	SplineMeshComponent[SplineIndex]->SetStaticMesh(mesh);
	//Allow the mesh to be moved and dynamically render shadows as it is continously constructed.
	SplineMeshComponent[SplineIndex]->SetMobility(EComponentMobility::Movable);
	//Ensure that construction is done using this method
	SplineMeshComponent[SplineIndex]->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	//Register the mesh to the world to have it spawn during runtime
	SplineMeshComponent[SplineIndex]->RegisterComponentWithWorld(GetWorld());
	//Add spline node relative to the curve
	SplineMeshComponent[SplineIndex]->AttachToComponent(GetSpline(), FAttachmentTransformRules::KeepRelativeTransform);
	//Set the start and end point for the mesh
	SplineMeshComponent[SplineIndex]->SetStartAndEnd(StartingPoint, StartTangent, EndPoint, EndTangent);
	//Enable collision for the spline mesh
	SplineMeshComponent[SplineIndex]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); //Note: consider changing this if physics is not being used
	//Rotate mesh in the direction of the spline's forward Axis
	SplineMeshComponent[SplineIndex]->SetForwardAxis(ForwardAxis);

	SplineMeshComponent[SplineIndex]->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
	SplineMeshComponent[SplineIndex]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	if (DefaultMaterial)
		SplineMeshComponent[SplineIndex]->SetMaterial(0, DefaultMaterial);

	if (tilesPerRow > 1)
	{ // TEMP
		for (int i = 1; i < tilesPerRow; i++)
		{
			SplineMeshComponent.Emplace(NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass()));
			SplineMeshComponent[SplineIndex + i]->SetStaticMesh(mesh);
			SplineMeshComponent[SplineIndex + i]->SetMobility(EComponentMobility::Movable);
			SplineMeshComponent[SplineIndex + i]->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			SplineMeshComponent[SplineIndex + i]->RegisterComponentWithWorld(GetWorld());
			SplineMeshComponent[SplineIndex + i]->AttachToComponent(GetSpline(), FAttachmentTransformRules::KeepRelativeTransform);
			SplineMeshComponent[SplineIndex + i]->SetStartAndEnd(StartingPoint, StartTangent, EndPoint, EndTangent);
			SplineMeshComponent[SplineIndex + i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); //Note: consider changing this if physics is not being used
			SplineMeshComponent[SplineIndex + i]->SetForwardAxis(ForwardAxis);

			SplineMeshComponent[SplineIndex + i]->AddLocalOffset(GetSpline()->GetRightVectorAtSplinePoint(GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::Local) * (tileOffset * (1 - (2 * (i % 2)))));

			SplineMeshComponent[SplineIndex]->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;

			if (DefaultMaterial)
				SplineMeshComponent[SplineIndex]->SetMaterial(0, DefaultMaterial);
		}
	}
}

void AMeshSplineActor::AddSplinePointAndMesh(const FVector newPointLocation, int meshType)
{
	AddSplinePoint(newPointLocation, true);
	RemoveAllSplineMesh(false);
	OnConstruction(GetActorTransform());
}

void AMeshSplineActor::RemoveFirstSplinePointAndMesh(bool bRemovePoint)
{
	RemoveSplineMesh(0, bRemovePoint);

	if (tilesPerRow > 1)
	{
		for (int i = 1; i < tilesPerRow; i++)
		{
			RemoveSplineMesh(0, false);
		}
	}
}

void AMeshSplineActor::SetDefaultMesh(UStaticMesh* StaticMesh)
{
	DefaultMesh = StaticMesh;
}

void AMeshSplineActor::RemoveAllSplineMesh(bool bRemovePoints)
{
	int numMeshToRemove = SplineMeshComponent.Num();
	for (int i = 0; i < numMeshToRemove; i++)
	{
		RemoveSplineMesh(0, bRemovePoints);
	}
}

void AMeshSplineActor::RemoveSplineMesh(int index, bool bRemovePoint)
{
	if (SplineMeshComponent.Num() <= 0) return;

	SplineMeshComponent[index]->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	SplineMeshComponent[index]->UnregisterComponent();
	SplineMeshComponent[index]->DestroyComponent();
	SplineMeshComponent.RemoveAt(index);

	if(bRemovePoint)
		RemoveSplinePoint(index, true);
}
