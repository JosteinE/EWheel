// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Objects/ObjectActorBase.h"

// Sets default values
AObjectActorBase::AObjectActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void AObjectActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObjectActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

