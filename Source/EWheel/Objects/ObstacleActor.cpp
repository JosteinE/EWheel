// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Objects/ObstacleActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "EWheel/PlayerPawn.h"

// Sets default values
AObstacleActor::AObstacleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//FString tempPath = "StaticMesh'/Engine/BasicShapes/Cube.Cube'";
	//static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(*tempPath);
	//if (MeshAsset.Succeeded())
	//	SetStaticMesh(MeshAsset.Object);
}

void AObstacleActor::SetStaticMesh(UStaticMesh* inMesh)
{
	SetStaticMesh(GetMeshComponent(), inMesh);
}

void AObstacleActor::SetStaticMesh(UStaticMeshComponent* meshComp, UStaticMesh* inMesh)
{
	meshComp->SetStaticMesh(inMesh);
	meshComp->SetCollisionProfileName("PawnObstacle");
	meshComp->SetNotifyRigidBodyCollision(true); // Simulation Generates Hit Events
	meshComp->SetMobility(EComponentMobility::Movable);

	meshComp->OnComponentHit.AddDynamic(this, &AObstacleActor::OnMeshHit);
}

void AObstacleActor::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Mirror the colliding actor's forward vector from the impact point and add impulse in that direction.
	float collisionAngle = FVector::DotProduct(OtherActor->GetActorForwardVector(), Hit.ImpactNormal);
	FVector impulseDirection = OtherActor->GetActorForwardVector() - 2 * collisionAngle * Hit.ImpactNormal;

	if (FMath::RadiansToDegrees(collisionAngle) > MaxToleratedAngle)
		Cast<APlayerPawn>(OtherActor)->KillPlayer();
	else
		Cast<APlayerPawn>(OtherActor)->AddToScore(1);
}