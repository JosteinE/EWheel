// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshSplineActor.h"
#include "UObject/ConstructorHelpers.h"
#include "PhysicsEngine/BodySetup.h"

#include "EWheel/MeshGenerator.h"
#include "EWheel/Spline/SplineTilePicker.h"

AMeshSplineActor::AMeshSplineActor()
{
	MeshGen = new MeshGenerator;
	TilePicker = new SplineTilePicker;
	TilePicker->SetNumRowsToLog(3);
}

AMeshSplineActor::~AMeshSplineActor()
{
	delete MeshGen;
	MeshGen = nullptr;
	delete TilePicker;
	TilePicker = nullptr;
}

void AMeshSplineActor::OnConstruction(const FTransform& Transform)
{
	// Construct the spline mesh
	for (int SplineCount = 0; SplineCount < GetSpline()->GetNumberOfSplinePoints() - 1; SplineCount++)
	{
		ConstructMesh(SplineCount);
	}
}

void AMeshSplineActor::ConstructMesh(int SplineIndex)
{
	// Ensure that our mesh exists, otherwise return
	UStaticMesh* mesh = MeshGen->StitchStaticMesh(TilePicker->GetNewTiles(3));
	if (!mesh) return;

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
}

void AMeshSplineActor::AddSplinePointAndMesh(const FVector newPointLocation)
{
	AddSplinePoint(newPointLocation, true);

	if (SplineMeshComponent.Num() < numRowsToReConPostInit)
	{
		ConstructMesh(SplineMeshComponent.Num());
		return;
	}

	int reconEnd = SplineMeshComponent.Num();
	int reconStart = reconEnd - numRowsToReConPostInit;
	
	// Remove the mesh that is to be reconstructed
	for (int i = reconEnd - 1; i > reconStart - 1; i--)
	{
		RemoveSplineMesh(i, false);
	}

	// Reconstruct mesh
	for (int i = reconStart; i <= reconEnd; i++)
	{
		ConstructMesh(i);
	}
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
	//DefaultMesh = StaticMesh;
}

void AMeshSplineActor::RemoveAllSplineMesh(bool bRemovePoints)
{
	for (int i = 0; i < SplineMeshComponent.Num(); i++)
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
