// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/SplineActor.h"
#include "Components/SplineComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ASplineActor::ASplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
	if (SplineComponent)
		RootComponent = SplineComponent;

	Mesh = CreateDefaultSubobject<UStaticMesh>(TEXT("SplineStaticMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Meshes/TempPlayerWheel.TempPlayerWheel'"));
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
		//Register the node to the world to have it spawn during runtime
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
		const FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);

		//Connect the two points
		SplineMeshComponent->SetStartAndEnd(StartingPoint, StartTangent, EndPoint, StartTangent);
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

