// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshSplineActor.h"
#include "UObject/ConstructorHelpers.h"
#include "PhysicsEngine/BodySetup.h"
#include "EWheel/Spline/SplineTilePicker.h"

AMeshSplineActor::AMeshSplineActor()
{
	TilePicker = new SplineTilePicker;
	TilePicker->SetNumRowsToLog(2);
}

AMeshSplineActor::~AMeshSplineActor()
{
	delete TilePicker;
	TilePicker = nullptr;
}

void AMeshSplineActor::OnConstruction(const FTransform& Transform)
{
	//// Construct the spline mesh
	//for (int SplineCount = 0; SplineCount < GetSpline()->GetNumberOfSplinePoints() - 1; SplineCount++)
	//{
	//	ConstructMesh(SplineCount);
	//}
}

void AMeshSplineActor::ConstructMesh(int SplineIndex, UStaticMesh* inMesh, int rot, float offset)
{
	// Ensure that our mesh exists, otherwise return
	UStaticMesh* mesh;
	if (inMesh)
		mesh = inMesh;
	else
	{
		mesh = TilePicker->GetNewTiles(tilesPerRow)[1];// MeshGen->StitchStaticMesh(TilePicker->GetLastRowRotation(tilesPerRow), TilePicker->GetNewTiles(tilesPerRow));
		if (!mesh) return;
	}

	//Use the previous point as the starting location
	FVector StartingPoint = GetSpline()->GetLocationAtSplinePoint(SplineIndex, ESplineCoordinateSpace::Local);
	//Get the tangent belonging to our previous point
	FVector StartTangent = GetSpline()->GetTangentAtSplinePoint(SplineIndex, ESplineCoordinateSpace::Local);
	//Get the next point along our curve
	FVector EndPoint = GetSpline()->GetLocationAtSplinePoint(SplineIndex + 1, ESplineCoordinateSpace::Local);
	//Get the tangent belonging to our next point
	FVector EndTangent = GetSpline()->GetTangentAtSplinePoint(SplineIndex + 1, ESplineCoordinateSpace::Local);

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
	if (rot == 1 || rot == 3)
	{	// Rotate the mesh sideways (-90 degrees)
		SplineMeshComponent[SplineIndex]->SetForwardAxis(ESplineMeshAxis::Y, false);
		SplineMeshComponent[SplineIndex]->SetStartRoll(-1.5708, false);
		SplineMeshComponent[SplineIndex]->SetEndRoll(-1.5708, false);
	}
	// Inverse the mesh needs to be rotated 90 or 180 degrees
	if (rot == 1 || rot == 2)
	{
		SplineMeshComponent[SplineIndex]->SetStartOffset(FVector2D{ -offset, 0 }, false);
		SplineMeshComponent[SplineIndex]->SetEndOffset(FVector2D{ -offset, 0 }, false);
		SplineMeshComponent[SplineIndex]->SetStartAndEnd(EndPoint, -EndTangent, StartingPoint, -StartTangent);
	}
	else
	{
		SplineMeshComponent[SplineIndex]->SetStartOffset(FVector2D{ offset, 0 }, false);
		SplineMeshComponent[SplineIndex]->SetEndOffset(FVector2D{ offset, 0 }, false);
		SplineMeshComponent[SplineIndex]->SetStartAndEnd(StartingPoint, StartTangent, EndPoint, EndTangent);
	}

	//Enable collision for the spline mesh
	SplineMeshComponent[SplineIndex]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	SplineMeshComponent[SplineIndex]->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
	SplineMeshComponent[SplineIndex]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	SplineMeshComponent[SplineIndex]->SetCastShadow(false);

	if (DefaultMaterial)
		SplineMeshComponent[SplineIndex]->SetMaterial(0, DefaultMaterial);
}

void AMeshSplineActor::AddSplinePointAndMesh(const FVector newPointLocation)
{	
	AddSplinePoint(newPointLocation, false);
	ConstructMesh(SplineMeshComponent.Num(), nullptr, FMath::RandRange(0, 3));
}

void AMeshSplineActor::AddSplinePointAndMesh(const FVector newPointLocation, UStaticMesh* staticMesh, int rot, float offset)
{
	AddSplinePoint(newPointLocation, false);
	ConstructMesh(SplineMeshComponent.Num(), staticMesh, rot, offset);
}

void AMeshSplineActor::RemoveFirstSplinePointAndMesh(bool bRemovePoint)
{
	RemoveSplineMesh(0, bRemovePoint);
}

void AMeshSplineActor::SetDefaultMesh(UStaticMesh* StaticMesh)
{
	DefaultMesh = StaticMesh;
}

void AMeshSplineActor::SetDefaultMaterial(UMaterialInterface* Material)
{
	DefaultMaterial = Material;
}

void AMeshSplineActor::SetNumTilesPerRow(int numTiles)
{
	tilesPerRow = numTiles;
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
