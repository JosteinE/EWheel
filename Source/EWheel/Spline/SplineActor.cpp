// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/SplineActor.h"

// Sets default values
ASplineActor::ASplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
	if (SplineComponent)
		RootComponent = SplineComponent;
}

void ASplineActor::OnConstruction(const FTransform& Transform)
{

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
	SplineComponent->AddSplineWorldPoint(newPointLocation);
	if (bUpdateSpline)
		SplineComponent->UpdateSpline();
}

void ASplineActor::RemoveFirstSplinePoint(bool bUpdateSpline)
{
	RemoveSplinePoint(0, bUpdateSpline);
}

void ASplineActor::RemoveSplinePoint(int index, bool bUpdateSpline)
{
	if (SplineComponent->GetNumberOfSplinePoints() <= 0 || index > SplineComponent->GetNumberOfSplinePoints()) return;

	SplineComponent->RemoveSplinePoint(index, bUpdateSpline);
}

