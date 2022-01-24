// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EWheel/Spline/SplineActor.h"
#include "Components/SplineMeshComponent.h"
#include "MeshSplineActor.generated.h"

/**
 * 
 */
UCLASS()
class EWHEEL_API AMeshSplineActor : public ASplineActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SplineMesh")
	TArray<USplineMeshComponent*> SplineMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* DefaultMesh;

	// Using TEnumAsByte to expose the enum to blueprints. This lets us rotate our mesh to match the forward direction of the spline
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* DefaultMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* AlternativeMaterial = nullptr;

	AMeshSplineActor();

protected:
	void OnConstruction(const FTransform& Transform) override;

	void ConstructMesh(int SplineIndex, int MeshType = 0);

public:
	// Adds a point to the spline
	void AddSplinePointAndMesh(const FVector newPointLocation, int meshType = 0);
	// Removes the first point along the spline
	void RemoveFirstSplinePointAndMesh(bool bRemovePoint = true);

private:
	void RemoveAllSplineMesh(bool bRemovePoints = true);
	void RemoveSplineMesh(int index, bool bRemovePoint = true);

	int numMeshToReConPostInit = 0;
	int tilesPerRow = 3;
};
