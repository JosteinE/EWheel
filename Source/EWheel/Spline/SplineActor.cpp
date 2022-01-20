// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/SplineActor.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASplineActor::ASplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
	if (SplineComponent)
		RootComponent = SplineComponent;

	//If there is trouble converting this actor from code to blueprint, comment this out!
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'"));
	if (MeshAsset.Succeeded())
		Mesh = MeshAsset.Object;
}

void ASplineActor::OnConstruction(const FTransform& Transform)
{
	if (!Mesh) return;

	for (int SplineCount = 0; SplineCount < SplineComponent->GetNumberOfSplinePoints() - 1; SplineCount++)
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

		SplineMeshComponent->SetStaticMesh(Mesh);
		//Allowing for the mesh to be moved and dynamically render shadows as it is continously constructed.
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		//Ensure that construction is done using this method
		SplineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		//Register the mesh to the world to have it spawn during runtime
		SplineMeshComponent->RegisterComponentWithWorld(GetWorld());
		//Add spline node relative to the curve
		SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);

		//Use the previous node as a starting point
		const FVector StartingPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
		//Get the tangent belonging to our previous node
		const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
		//Create a next node along our curve
		const FVector EndPoint = SplineComponent->GetLocationAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);
		//Get the tangent belonging to our new node
		const FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);

		//Connect the two points
		SplineMeshComponent->SetStartAndEnd(StartingPoint, StartTangent, EndPoint, EndTangent);
		//Enable collision for the spline mesh
		SplineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); //Note: consider changing this if physics is not being used

		//Rotate mesh in the direction of the spline's forward Axis
		SplineMeshComponent->SetForwardAxis(ForwardAxis);

		//Apply material
		if(DefaultMaterial)
			SplineMeshComponent->SetMaterial(0, DefaultMaterial);
	}
}

// Called when the game starts or when spawned
void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASplineActor::AddSplinePoint(const FVector newPointLocation, bool bUpdateSpline)
{
	SplineComponent->AddSplineLocalPoint(newPointLocation);
	UE_LOG(LogTemp, Warning, TEXT("SplinePoint Added"));

	if (bUpdateSpline)
	{
		SplineComponent->UpdateSpline();
		OnConstruction(GetActorTransform());
	}
}

void ASplineActor::RemoveFirstSplinePoint(bool bUpdateSpline)
{
	FVector splineToRemoveLoc = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	UE_LOG(LogTemp, Warning, TEXT("Removing spline point: %f, %f, %f "), splineToRemoveLoc.X, splineToRemoveLoc.Y, splineToRemoveLoc.Z);
	SplineComponent->RemoveSplinePoint(0, bUpdateSpline);

	if (bUpdateSpline)
	{
		OnConstruction(GetActorTransform());
	}
}

