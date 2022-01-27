// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/GameModes/EndlessGameMode.h"
#include "EWheel/PlayerPawn.h"
#include "EWheel/Spline/MeshSplineActor.h"
#include "EWheel/MeshGenerator.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AEndlessGameMode::AEndlessGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>ObjectMeshAsset(TEXT("StaticMesh'/Game/Meshes/PointObject.PointObject'"));
	if (ObjectMeshAsset.Succeeded())
		PointObjectMesh = ObjectMeshAsset.Object;
}

void AEndlessGameMode::BeginPlay()
{
	// Spawn the player
	TSubclassOf<APawn> pawnClass = LoadObject<UBlueprint>(NULL, TEXT("Blueprint'/Game/Blueprints/PlayerPawnBP.PlayerPawnBP'"))->GeneratedClass;
	FActorSpawnParameters playerSpawnParams;
	playerSpawnParams.Owner = this;
	playerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	mainPlayer = GetWorld()->SpawnActor<APawn>(pawnClass, FVector{ 0,0,0 }, FRotator{ 0,0,0 }, playerSpawnParams);
	GetWorld()->GetFirstPlayerController()->Possess(mainPlayer);

	// Spawn the path
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;
	mainPath = GetWorld()->SpawnActor<AMeshSplineActor>(AMeshSplineActor::StaticClass(), FVector{ 0.f, 0.f, splineSpawnVerticalOffset }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);

	//TEST
	TArray<FString> meshPaths;
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_150x150.Ground_Pit_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/DefaultGround_150x150_Sub.DefaultGround_150x150_Sub'");
	MeshGenerator meshGen;
	mainPath->SetDefaultMesh(meshGen.GenerateStaticMeshFromTile(meshPaths));
}

void AEndlessGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the player is further than the max distance from the last point, add a new point.
	if ((lastSplinePointLoc - mainPlayer->GetActorLocation()).Size() < minDistToLastSplinePoint)
	{
		ExtendPath();

		tileSpawnedCounter++;
		if (tileSpawnedCounter > 3)
		{
			FVector previousSplinePointLoc = mainPath->GetSpline()->GetWorldLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 2);
			SpawnPointObject(previousSplinePointLoc + (lastSplinePointLoc - previousSplinePointLoc) * 0.5f + FVector{ 0.f, 0.f, 50.f });

			tileSpawnedCounter = 0;
		}
	}

	if (PointObject)
		PointObject->AddActorWorldRotation(FRotator{ 0.f, 100.f * DeltaTime, 0.f });
}

void AEndlessGameMode::ExtendPath()
{
	const FVector LastSPlinePointDirection = mainPath->GetSpline()->GetDirectionAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	const FVector newLocation = mainPlayer->GetActorLocation() + mainPlayer->GetActorForwardVector() * FVector { distToNextSplinePoint, distToNextSplinePoint, 0 };

	// Remove the first point in the spline if adding 1 exceedes the max number of spline points.
	if (mainPath->GetSpline()->GetNumberOfSplinePoints() + 1 > maxNumSplinePoints)
	{
		mainPath->AddSplinePointAndMesh(newLocation, 0);
		mainPath->RemoveFirstSplinePointAndMesh(true);
	}
	else
		mainPath->AddSplinePointAndMesh(newLocation, 0);

	lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
}

void AEndlessGameMode::SpawnPointObject(FVector& location)
{
	FActorSpawnParameters PointObjectSpawnParams;
	PointObjectSpawnParams.Owner = this;
	PointObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PointObject = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FVector(), FRotator(), PointObjectSpawnParams);
	PointObjectMeshComponent = NewObject<UStaticMeshComponent>(PointObject, UStaticMeshComponent::StaticClass());
	PointObject->SetRootComponent(PointObjectMeshComponent);
	PointObjectMeshComponent->SetStaticMesh(PointObjectMesh);
	PointObjectMeshComponent->SetMobility(EComponentMobility::Movable);
	PointObjectMeshComponent->SetWorldScale3D(FVector{ 0.33f, 0.33f, 0.33f });
	PointObject->SetActorLocation(location);
	PointObjectMeshComponent->SetRelativeRotation(FRotator{ 90.f, 0.f, 0.f });
	PointObjectMeshComponent->RegisterComponent();
}



void AEndlessGameMode::EndGame()
{
}
