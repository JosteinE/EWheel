// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Objects/PickUpActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"

#include "EWheel/PlayerPawn.h"

// Sets default values
APickUpActor::APickUpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = MeshComponent;
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

	MeshComponent->AddWorldRotation(FRotator{ 0.f, 100.f * DeltaTime, 0.f });
}

void APickUpActor::SetStaticMesh(FString& inPath)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(*inPath);
	if (MeshAsset.Succeeded())
		SetStaticMesh(MeshAsset.Object);
}

void APickUpActor::SetStaticMesh(UStaticMesh* inMesh)
{
	MeshComponent->SetStaticMesh(inMesh);
	MeshComponent->SetCollisionProfileName("IgnoreAllOverlapPawn");
	MeshComponent->SetGenerateOverlapEvents(true);
	MeshComponent->SetMobility(EComponentMobility::Movable);

	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &APickUpActor::OnOverlapBegin);

	SetActorTickEnabled(true);
}

void APickUpActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Cast<APlayerPawn>(OtherActor)->AddToScore(1);
	MeshComponent->SetVisibility(false);
	MeshComponent->SetCollisionProfileName("NoCollision");
	// Will be destroyed in the game mode
}

