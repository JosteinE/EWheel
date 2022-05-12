// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EWheel/Objects/ObjectActorBase.h"
#include "ObstacleActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class EWHEEL_API AObstacleActor : public AObjectActorBase
{
	GENERATED_BODY()

	/** Boosts the player in the reflected forward direction when colliding at a tolerated angle */
	UPROPERTY(Category = Collision, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ImpulseStrength = 100.f;

	/** The minimum angle that will be tolerated upon collision. The player vehicle will be stopped if the angle is lower */
	UPROPERTY(Category = Collision, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxToleratedAngle = 45.f;

	/** The number of points awarded to the player per second during contact with this actor */
	UPROPERTY(Category = StylePoint, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int PointPerSec = 5;

public:	
	// Sets default values for this actor's properties
	AObstacleActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetStaticMesh(UStaticMesh* inMesh) override;
	void SetStaticMesh(UStaticMeshComponent* meshComp, UStaticMesh* inMesh);

private:
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Used to delay when a point is given based on time, rather than individual ticks.
	float CollisionTimeTracker = 0;
};
