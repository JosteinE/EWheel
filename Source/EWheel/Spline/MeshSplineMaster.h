// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshSplineMaster.generated.h"

class AMeshSplineActor;
class SplineTilePicker;
class UStaticMesh;
class UObjectSpawner;
/** Master of splines
 *  Spawns multiple splines ontop itself then offsets the splines individual tiles to maintain correct mesh deformation
 */
UCLASS()
class EWHEEL_API AMeshSplineMaster : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* DefaultMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMesh", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* AlternativeMaterial = nullptr;

public:
	AMeshSplineMaster();
	~AMeshSplineMaster();

	void SetMaxNumSplinePoints(int maxNum);

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
	AMeshSplineActor* AddSpline(int index, bool bEmplaceToArray = true); 

	/// Detaches and removes splines from the master in the following order: Right then left
	void RemoveSplines(int num);
	void RemoveSpline();

	FVector GenerateNewPointLocation();
	// Adds a new point to every spline
	void AddPoint(FVector location);

	void SpawnObjectsLastRow();

	// Removes the first point on every spline
	void RemoveFirstSplinePointAndMesh(int splineIndex);

	int GetNumSplinePoints();
	bool GetIsAtMaxSplinePoints();

	float GetDefaultSplineOffset(int splineIndex);

	//TEMP
	FVector GetLocationAtSplinePoint(int pointIndex);
private:
	TArray<AMeshSplineActor*> mSplines;
	SplineTilePicker* mTilePicker;
	UObjectSpawner* mObjectSpawner;

	// Index of the controlling spline
	int mMasterSplineIndex = 0;
	
	// Construction details
	bool bAddEdges = true;
	float mTileSize = 150.f;
	int mMaxNumSplinePoints = 20;
	float mSplineSpawnVerticalOffset = 0.f;

	// Path extention limits 
	float mSplineVerticalMin = -50.f;
	float mSplineVerticalMax = 50.f;
	float mSplineVerticalStep = 20.f;

public:
	FORCEINLINE void SetTileSize(int tileSize) { mTileSize = tileSize; };
	// Sets the spline for every other spline to follow
	FORCEINLINE void SetMasterSpline(int splineIndex) { mMasterSplineIndex = splineIndex; };

	FORCEINLINE void SetDefaultMaterial(UMaterialInterface* Material) { DefaultMaterial = Material; };
	FORCEINLINE void SetAlternativeMaterial(UMaterialInterface* Material) { AlternativeMaterial = Material; };
};
