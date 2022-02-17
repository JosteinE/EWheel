// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Objects/PickUpActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

#include "EWheel/PlayerPawn.h"

// Sets default values
APickUpActor::APickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

// Called when the game starts or when spawned
void APickUpActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickUpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetMeshComponent()->AddWorldRotation(FRotator{ 0.f, 100.f * DeltaTime, 0.f });
}

void APickUpActor::SetStaticMesh(UStaticMesh* inMesh)
{
	GetMeshComponent()->SetStaticMesh(inMesh);
	GetMeshComponent()->SetCollisionProfileName("IgnoreAllOverlapPawn");
	GetMeshComponent()->SetGenerateOverlapEvents(true);
	GetMeshComponent()->SetMobility(EComponentMobility::Movable);

	GetMeshComponent()->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::OnOverlapBegin);

	SetActorTickEnabled(true);
}

void APickUpActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Cast<APlayerPawn>(OtherActor)->AddToScore(1);
	GetMeshComponent()->SetVisibility(false);
	GetMeshComponent()->SetCollisionProfileName("NoCollision");
	// Will be destroyed in the game mode
}

