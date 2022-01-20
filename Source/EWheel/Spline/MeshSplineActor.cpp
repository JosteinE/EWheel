// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshSplineActor.h"
#include "UObject/ConstructorHelpers.h"

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

	//Use the previous node as a starting point
	const FVector StartingPoint = GetSpline()->GetLocationAtSplinePoint(SplineIndex, ESplineCoordinateSpace::Local);
	//Get the tangent belonging to our previous node
	const FVector StartTangent = GetSpline()->GetTangentAtSplinePoint(SplineIndex, ESplineCoordinateSpace::Local);
	//Create a next node along our curve
	const FVector EndPoint = GetSpline()->GetLocationAtSplinePoint(SplineIndex + 1, ESplineCoordinateSpace::Local);
	//Get the tangent belonging to our new node
	const FVector EndTangent = GetSpline()->GetTangentAtSplinePoint(SplineIndex + 1, ESplineCoordinateSpace::Local);

	//Connect the two points
	SplineMeshComponent[SplineIndex]->SetStartAndEnd(StartingPoint, StartTangent, EndPoint, EndTangent);
	//Enable collision for the spline mesh
	SplineMeshComponent[SplineIndex]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); //Note: consider changing this if physics is not being used

	//Rotate mesh in the direction of the spline's forward Axis
	SplineMeshComponent[SplineIndex]->SetForwardAxis(ForwardAxis);

	//Apply material
	if (DefaultMaterial)
		SplineMeshComponent[SplineIndex]->SetMaterial(0, DefaultMaterial);
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
}

void AMeshSplineActor::RemoveAllSplineMesh(bool bRemovePoints)
{
	for (int i = 0; i < SplineMeshComponent.Num(); i++)
	{
		RemoveSplineMesh(i, bRemovePoints);
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
