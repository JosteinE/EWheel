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

	// Returns an appropriate location for the next spline point. 
	FVector GenerateNewPointLocation();
	
	// Adds a new point to every spline
	void AddPoint(FVector location);

	// Calls the objectSpawner and spawns obstacles where appropriate on the last row
	void SpawnObjectsLastRow();

	// Removes the first point on every spline
	void RemoveFirstSplinePointAndMesh(int splineIndex);

	// Returns number of spline points of the master spline
	int GetNumSplinePoints();

	// Checks if the master spline is at the max number of points
	bool GetIsAtMaxSplinePoints();

	// Returns the default offset for the given spline.
	float GetDefaultSplineOffset(int splineIndex);

	//######################
	// PATH PROPERTY SETTERS
	//######################

	// Sets the max number of spline points. Any points added after reaching this number will remove the oldest point. 
	void SetMaxNumSplinePoints(int maxNum);

	// Sets the obstacle spawn chance in the ObjectSpawner
	void SetObstacleSpawnChance(int obstacleSpawnChance);

	// Sets the point spawn chance in the ObjectSpawner
	void SetPointSpawnChance(int pointSpawnChance);

	// Sets the Powerup spawn chance in the ObjectSpawner
	void SetPowerUpSpawnChance(int powerUpSpawnChance);

	// Enables the pit tile set
	void SetSpawnPits(bool spawnPits);

	// Enables the ramp tile set
	void SetSpawnRamps(bool spawnRamps);

	// Enables the hole tile set
	void SetSpawnHoles(bool spawnHoles);

	// Enables the high resolution models
	void SetUseHighResModels(bool highResModels);

	void LoadFromJson(TSharedPtr<FJsonObject> inJson);

	//TEMP (for use in game mode)
	FVector GetLocationAtSplinePoint(int pointIndex);
	void GetLocationAndRotationAtSplinePoint(FVector& returnLocation, FRotator& returnRotation, int pointIndex);
	float FindInputKeyClosestToWorldLocation(FVector& location);
private:
	TArray<AMeshSplineActor*> mSplines;
	SplineTilePicker* mTilePicker;
	UObjectSpawner* mObjectSpawner;

	// Index of the controlling spline
	int mMasterSplineIndex = 0;
	
	// Construction details
	bool bAddEdges = true;
	bool bSpawnObjects = true;
	float mTileSize = 150.f;
	int mMaxNumSplinePoints = 20;
	float mSplineSpawnVerticalOffset = 0.f;

	// Path extention limits 
	float mSplineVerticalMin = -50.f;
	float mSplineVerticalMax = 50.f;
	float mSplineVerticalStep = 20.f;

public:
	// Sets the spline for every other spline to follow
	FORCEINLINE void SetMasterSpline(int splineIndex) { mMasterSplineIndex = splineIndex; };

	FORCEINLINE void SetTileSize(int tileSize) { mTileSize = tileSize; };
	FORCEINLINE void SetDefaultMaterial(UMaterialInterface* Material) { DefaultMaterial = Material; };
	FORCEINLINE void SetAlternativeMaterial(UMaterialInterface* Material) { AlternativeMaterial = Material; };
};
