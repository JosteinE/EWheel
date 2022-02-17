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
	GetMeshComponent()->SetStaticMesh(inMesh);
	GetMeshComponent()->SetCollisionProfileName("PawnObstacle");
	GetMeshComponent()->SetNotifyRigidBodyCollision(true); // Simulation Generates Hit Events
	GetMeshComponent()->SetMobility(EComponentMobility::Movable);

	GetMeshComponent()->OnComponentHit.AddDynamic(this, &AObstacleActor::OnMeshHit);
}

void AObstacleActor::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Mirror the colliding actor's forward vector from the impact point and add impulse in that direction.
	float collisionAngle = FVector::DotProduct(OtherActor->GetActorForwardVector(), Hit.ImpactNormal);
	FVector impulseDirection = OtherActor->GetActorForwardVector() - 2 * collisionAngle * Hit.ImpactNormal;

	if (FMath::RadiansToDegrees(collisionAngle) > MaxToleratedAngle)
		Cast<APlayerPawn>(OtherActor)->KillPlayer();//OtherComp->AddImpulse(OtherActor->GetActorForwardVector() * -100000.f);
}

