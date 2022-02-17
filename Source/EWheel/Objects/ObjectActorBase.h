// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "ObjectActorBase.generated.h"

UCLASS()
class EWHEEL_API AObjectActorBase : public AActor
{
	GENERATED_BODY()
	
	/** Mesh component belonging to the obstacle */
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

public:	
	// Sets default values for this actor's properties
	AObjectActorBase();

	void SetStaticMesh(FString& inPath);

	// Tile index on a row (from left to right)
	int mTileIndex = 0;
protected:
	virtual void SetStaticMesh(UStaticMesh* inMesh);

public:	
	/** Returns the object's mesh component **/
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
};
