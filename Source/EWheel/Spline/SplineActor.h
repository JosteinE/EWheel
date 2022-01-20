// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "Components/SplineComponent.h"
#include "SplineActor.generated.h"

UCLASS(Blueprintable)
class EWHEEL_API ASplineActor : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category = "Spline")
	USplineComponent* SplineComponent;

public:
	// Sets default values for this actor's properties
	ASplineActor();

protected:

	// 
	void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Adds a point to the spline
	void AddSplinePoint(const FVector newPointLocation, bool bUpdateSpline = true);

	// Removes the first point along the spline
	void RemoveFirstSplinePoint(bool bUpdateSpline = true);

	void RemoveSplinePoint(int index, bool bUpdateSpline = true);

	/** Returns the player mesh subobject **/
	FORCEINLINE USplineComponent* GetSpline() const { return SplineComponent; }
}; 
