// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Objects/ObstacleActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AObstacleActor::AObstacleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = MeshComponent;

	FString tempPath = "StaticMesh'/Engine/BasicShapes/Cube.Cube'";
	SetStaticMesh(tempPath);
}

void AObstacleActor::SetStaticMesh(FString& inPath)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(*inPath);
	if (MeshAsset.Succeeded())
		SetStaticMesh(MeshAsset.Object);
}

void AObstacleActor::SetStaticMesh(UStaticMesh* inMesh)
{
	MeshComponent->SetStaticMesh(inMesh);
	MeshComponent->SetCollisionProfileName("PawnObstacle");
	MeshComponent->SetNotifyRigidBodyCollision(true); // Simulation Generates Hit Events
	MeshComponent->SetMobility(EComponentMobility::Movable);

	MeshComponent->OnComponentHit.AddDynamic(this, &AObstacleActor::OnMeshHit);
}

void AObstacleActor::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Mirror the colliding actor's forward vector from the impact point and add impulse in that direction.
	float collisionAngle = FVector::DotProduct(OtherActor->GetActorForwardVector(), Hit.ImpactNormal);
	FVector impulseDirection = OtherActor->GetActorForwardVector() - 2 * collisionAngle * Hit.ImpactNormal;

	if(FMath::RadiansToDegrees(collisionAngle) < MinToleratedAngle)
		OtherComp->AddImpulse(impulseDirection * ImpulseStrength);
	else
		OtherComp->AddImpulse(OtherActor->GetActorForwardVector() * -100000.f);
}

