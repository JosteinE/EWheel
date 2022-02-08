// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EWheel/Spline/SplineActor.h"
#include "Components/SplineMeshComponent.h"
#include "EWheel/Spline/SplineTilePicker.h"
#include "MeshSplineActor.generated.h"

class MeshGenerator;
class SplineTilePicker;
/**
 * 
 */
UCLASS()
class EWHEEL_API AMeshSplineActor : public ASplineActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	TArray<USplineMeshComponent*> SplineMeshComponent;

	// Using TEnumAsByte to expose the enum to blueprints. This lets us rotate our mesh to match the forward direction of the spline
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* DefaultMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* AlternativeMaterial = nullptr;

	AMeshSplineActor();
	~AMeshSplineActor();
protected:
	void OnConstruction(const FTransform& Transform) override;

	void ConstructMesh(int SplineIndex);

public:
	// Adds a point to the spline
	void AddSplinePointAndMesh(const FVector newPointLocation);
	// Removes the first point along the spline
	void RemoveFirstSplinePointAndMesh(bool bRemovePoint = true);

	void SetDefaultMesh(UStaticMesh* StaticMesh);

private:
	void RemoveAllSplineMesh(bool bRemovePoints = true);
	void RemoveSplineMesh(int index, bool bRemovePoint = true);

	int numRowsToReConPostInit = 3; // SHOULD BE > 0
	int tilesPerRow = 1;
	float tileOffset = 150.f;

	// Stitches multiple StaticMesh tiles into one
	MeshGenerator* MeshGen;
	// Logs the last few rows and picks appropriate tiles for the next
	SplineTilePicker* TilePicker;
};
