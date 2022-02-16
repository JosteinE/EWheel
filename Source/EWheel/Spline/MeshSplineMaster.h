// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshSplineMaster.generated.h"

class AMeshSplineActor;
class SplineTilePicker;
class UStaticMesh;
/**
 * 
 */
UCLASS()
class EWHEEL_API AMeshSplineMaster : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* DefaultMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* AlternativeMaterial = nullptr;

	AMeshSplineMaster();
	~AMeshSplineMaster();

	// Initializes the splines
	void ConstructSplines(int numSplines);

	// Attatches a known spline to the master
	void AttatchSpline(int index);

	// Detatches a spline from the master, but keeps it in memory to later be reattatched
	void DetatchSpline(int index);

	// Adds and attatches new splines to the master in the following order: Left to right
	void AddSplines(int num);

	// Adds and attatches a new spline to the master on the right side
	// NB! If bEmplaceToArray = false, make sure to assign the returned ptr!
	AMeshSplineActor* AddSpline(bool bEmplaceToArray = true); 

	/// Detaches and removes splines from the master in the following order: Right then left
	void RemoveSplines(int num);
	void RemoveSpline();

	// Adds a new point to every spline
	void AddPoint(FVector location);

	// Removes the first point on every spline
	void RemoveFirstSplinePointAndMesh(int splineIndex);

private:
	TArray<AMeshSplineActor*> mSplines;
	SplineTilePicker* mTilePicker;

	bool bAddEdges = true;
	int mMasterSplineIndex = 0;
	float mSplineOffset = 150.f;
	int mMaxNumSplinePoints = 20;

public:
	// Sets the spline for every other spline to follow
	FORCEINLINE void SetSplineOffset(int splineOffset) { mSplineOffset = mSplineOffset; };

	// Sets the spline for every other spline to follow
	FORCEINLINE void SetMasterSpline(int splineIndex) { mMasterSplineIndex = splineIndex; };

	FORCEINLINE void SetDefaultMaterial(UMaterialInterface* Material) { DefaultMaterial = Material; };
	FORCEINLINE void SetAlternativeMaterial(UMaterialInterface* Material) { AlternativeMaterial = Material; };
};
