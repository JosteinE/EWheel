// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/GameModes/EndlessGameMode.h"
#include "EWheel/PlayerPawn.H"
#include "EWheel/Spline/MeshSplineActor.h"
#include "EWheel/MeshGenerator.h"

#include "Components/StaticMeshComponent.h"

AEndlessGameMode::AEndlessGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AEndlessGameMode::BeginPlay()
{
	// Spawn the player
	TSubclassOf<APawn> pawnClass = LoadObject<UBlueprint>(NULL, TEXT("Blueprint'/Game/Blueprints/PlayerPawnBP.PlayerPawnBP'"))->GeneratedClass;
	FActorSpawnParameters playerSpawnParams;
	playerSpawnParams.Owner = this;
	mainPlayer = GetWorld()->SpawnActor<APawn>(pawnClass, FVector{ 0,0,0 }, FRotator{ 0,0,0 }, playerSpawnParams);
	GetWorld()->GetFirstPlayerController()->Possess(mainPlayer);

	// Spawn the path
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;
	mainPath = GetWorld()->SpawnActor<AMeshSplineActor>(AMeshSplineActor::StaticClass(), FVector{ 0.f, 0.f, splineSpawnVerticalOffset }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);

	//TEST
	MeshGenerator meshGen;
	testMesh = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FVector{ 0,0,0 }, FRotator{ 0,0,0 }, playerSpawnParams);
	TArray<FString> meshPaths;
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_150x150.Ground_Pit_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/DefaultGround_150x150.DefaultGround_150x150'");
	UStaticMesh* testMeshMesh = meshGen.GenerateStaticMeshFromTile(meshPaths);
	UStaticMeshComponent* meshComp = NewObject<UStaticMeshComponent>(testMesh);
	meshComp->SetStaticMesh(testMeshMesh);
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->RegisterComponent();
	meshComp->CreateSceneProxy();

	UE_LOG(LogTemp, Warning, TEXT("FUCKING DID IT"));
}

void AEndlessGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the player is further than the max distance from the last point, add a new point.
	if ((lastSplinePointLoc - mainPlayer->GetActorLocation()).Size() < minDistToLastSplinePoint)
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
}



void AEndlessGameMode::EndGame()
{
}
