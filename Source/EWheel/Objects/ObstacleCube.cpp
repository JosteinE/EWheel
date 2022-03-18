// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Objects/ObstacleCube.h"

// Sets default values
AObstacleCube::AObstacleCube()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	
	FString meshPath = "StaticMesh'/Game/Meshes/Obstacles/ObstacleCube/ObstacleCube_Bot.ObstacleCube_Bot'";
	static ConstructorHelpers::FObjectFinder<UStaticMesh>BotAsset(*meshPath);
	if (BotAsset.Succeeded())
		SetStaticMesh(GetMeshComponent(), BotAsset.Object);

	MidMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MidStaticMeshComponent"));
	meshPath = "StaticMesh'/Game/Meshes/Obstacles/ObstacleCube/ObstacleCube_Mid.ObstacleCube_Mid'";
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MidAsset(*meshPath);
	if (MidAsset.Succeeded())
		SetStaticMesh(MidMeshComponent, MidAsset.Object);
	MidMeshComponent->SetupAttachment(GetMeshComponent());

	TopMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopStaticMeshComponent"));
	meshPath = "StaticMesh'/Game/Meshes/Obstacles/ObstacleCube/ObstacleCube_Top.ObstacleCube_Top'";
	static ConstructorHelpers::FObjectFinder<UStaticMesh>TopAsset(*meshPath);
	if (TopAsset.Succeeded())
		SetStaticMesh(TopMeshComponent, TopAsset.Object);
	TopMeshComponent->SetupAttachment(GetMeshComponent());
}

// Called every frame
void AObstacleCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AObstacleCube::OnConstruction(const FTransform& Transform)
{
	MidMeshComponent->SetRelativeLocation(FVector{ 0.f, 0.f, 7.8588f + 3.0325f * 0.5f * MidMeshComponent->GetComponentScale().Z });
	TopMeshComponent->SetRelativeLocation(FVector{ 0.f, 0.f, 7.8588f + 3.0325f * MidMeshComponent->GetComponentScale().Z });
}

void AObstacleCube::SetHeight(float height)
{
	MidMeshComponent->SetWorldScale3D(FVector{ MidMeshComponent->GetComponentScale().X, MidMeshComponent->GetComponentScale().Y, height });
	OnConstruction(GetTransform());
}