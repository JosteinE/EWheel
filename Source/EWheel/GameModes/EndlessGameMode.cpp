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

	this->HUDClass = LoadObject<UBlueprint>(NULL, TEXT("Blueprint'/Game/Blueprints/PlayerHud.PlayerHUD'"))->GeneratedClass;
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
	int pathIndex = mainPath->GetSpline()->GetNumberOfSplinePoints() - extendFromSplinePoint;
	if (pathIndex < 0)
		pathIndex = 0;

	if ((mainPath->GetSpline()->GetWorldLocationAtSplinePoint(pathIndex) - mainPlayer->GetActorLocation()).Size() < minDistToSplinePoint)
	{
		ExtendPath();

		// Spawn a point object
		if (FMath::RandRange(0, 99) < PointObjectSpawnChance)
		{
			FVector previousSplinePointLoc = mainPath->GetSpline()->GetWorldLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 2);

			int32 pointObjecOffset = FMath::RandRange(-1, 1);

			FVector pointObjectSpawnLocation = previousSplinePointLoc + (lastSplinePointLoc - previousSplinePointLoc) * 0.5f + FVector{ 0.f, 150.f * pointObjecOffset, 50.f };
			SpawnPointObject(pointObjectSpawnLocation);
		}
	}

	if (PointObject)
		PointObject->AddActorWorldRotation(FRotator{ 0.f, 100.f * DeltaTime, 0.f });
}

void AEndlessGameMode::ExtendPath()
{
	FVector LastSPlinePointDirection = mainPath->GetSpline()->GetDirectionAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	LastSPlinePointDirection.X += FMath::RandRange(0, 1000);
	LastSPlinePointDirection.Y += FMath::RandRange(0, 1000);
	LastSPlinePointDirection.Normalize();
	const FVector newLocation = mainPath->GetSpline()->GetWorldLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints()) * FVector { 1, 1, 0 } + LastSPlinePointDirection * FVector{ distToNextSplinePoint, distToNextSplinePoint, 0 };

	// Remove the first point in the spline if adding 1 exceedes the max number of spline points.
	mainPath->AddSplinePointAndMesh(newLocation, 0);
	if (mainPath->GetSpline()->GetNumberOfSplinePoints() > maxNumSplinePoints)
		mainPath->RemoveFirstSplinePointAndMesh(true);

	lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
}

void AEndlessGameMode::SpawnPointObject(FVector& location)
{
	FActorSpawnParameters PointObjectSpawnParams;
	PointObjectSpawnParams.Owner = this;
	PointObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	PointObject = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FVector(), FRotator(), PointObjectSpawnParams);
	PointObjectMeshComponent = NewObject<UStaticMeshComponent>(PointObject, UStaticMeshComponent::StaticClass(), TEXT("PointObjectMeshComponent"));
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
