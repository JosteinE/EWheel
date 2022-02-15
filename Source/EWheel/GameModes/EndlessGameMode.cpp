// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/GameModes/EndlessGameMode.h"
#include "EWheel/PlayerPawn.h"
#include "EWheel/Spline/MeshSplineActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

#include "EWheel/Objects/ObstacleActor.h"
#include "EWheel/Objects/PickUpActor.h"

#include "Kismet/GameplayStatics.h"

AEndlessGameMode::AEndlessGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	// Prevent the game mode from spawning in a garbage player.
	this->bStartPlayersAsSpectators = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>PointObjectAsset(TEXT("StaticMesh'/Game/Meshes/PointObject.PointObject'"));
	if (PointObjectAsset.Succeeded())
		PointObjectMesh = PointObjectAsset.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>ObstacleObjectAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (ObstacleObjectAsset.Succeeded())
		ObstacleMesh = ObstacleObjectAsset.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>DefaultMaterialAsset(TEXT("Material'/Game/Materials/GrassDirt_Material.GrassDirt_Material'"));
	if (DefaultMaterialAsset.Succeeded())
		DefaultMaterial = DefaultMaterialAsset.Object;

	this->HUDClass = LoadObject<UBlueprint>(NULL, TEXT("Blueprint'/Game/Blueprints/PlayerHud.PlayerHUD'"))->GeneratedClass;
}

void AEndlessGameMode::BeginPlay()
{
	// Spawn the path
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;
	mainPath = GetWorld()->SpawnActor<AMeshSplineActor>(AMeshSplineActor::StaticClass(), FVector{ 0.f, 0.f, splineSpawnVerticalOffset }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	mainPath->SetNumTilesPerRow(TilesPerRow);
	mainPath->SetDefaultMaterial(DefaultMaterial);

	// Create a starting area with no obstacles
	for (int i = 0; i < 5; i++)
	{
		ExtendPath();
	}

	// Spawn the player
	auto pawnClass = LoadObject<UBlueprint>(NULL, TEXT("Blueprint'/Game/Blueprints/PlayerPawnBP.PlayerPawnBP'"))->GeneratedClass;
	FActorSpawnParameters playerSpawnParams;
	playerSpawnParams.Owner = this;
	playerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	mainPlayer = GetWorld()->SpawnActor<APlayerPawn>(pawnClass, FVector{ 150.f,0, playerSpawnHeight }, FRotator{ 0,0,0 }, playerSpawnParams);
	GetWorld()->GetFirstPlayerController()->Possess(mainPlayer);

	//Bind Delegates
	Cast<APlayerPawn>(mainPlayer)->EscPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerEscapePressed);
	Cast<APlayerPawn>(mainPlayer)->RestartPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerRestartPressed);

	//TEST
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_BigRoot_150x150.Obstacle_BigRoot_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_Log_150x150.Obstacle_Log_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_RampStone_150x150.Obstacle_RampStone_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_Stone_150x150.Obstacle_Stone_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/Obstacles/Obstacle_Stump_150x150.Obstacle_Stump_150x150'");
}

void AEndlessGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the player is further than the max distance from the last point, add a new point.
	int pathIndex = extendFromSplinePoint;
	if (pathIndex > mainPath->GetSpline()->GetNumberOfSplinePoints() - 1)
		pathIndex = 0;

	// Extend the path if the player is within the minimum range to indexed spline point
	if (mainPath->GetSpline()->GetNumberOfSplinePoints() < maxNumSplinePoints || (mainPath->GetSpline()->GetWorldLocationAtSplinePoint(pathIndex) - mainPlayer->GetActorLocation()).Size() < minDistToSplinePoint)
	{
		ExtendPath();

		int numObstaclesSpawned = 0;
		// Check object spawn for each tile
		for (int i = 0; i < TilesPerRow; i++)
		{
			// Spawn a point object
			if (FMath::RandRange(0, 99) < PointSpawnChance)
			{
				FVector tileCentre = GetTileCentreLastRow(i);
				SpawnPointObject(tileCentre);
			}
			// Spawn Obstacle Object
			else if (!bSpawnedObstacleOnLast && numObstaclesSpawned < TilesPerRow - 1 && FMath::RandRange(0, 99) < ObstacleSpawnChance)
			{
				FVector tileCentre = GetTileCentreLastRow(i);
				SpawnObstacleObject(tileCentre);
				numObstaclesSpawned++;
			}
		}

		if (numObstaclesSpawned > 0)
			bSpawnedObstacleOnLast = true;
		else if (bSpawnedObstacleOnLast)
			bSpawnedObstacleOnLast = false;
	}
}

void AEndlessGameMode::ExtendPath()
{
	// Calculate the next spline point position
	FVector LastSPlinePointDirection = mainPath->GetSpline()->GetDirectionAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	FVector LastSplinePointRightVector = mainPath->GetSpline()->GetRightVectorAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	//LastSPlinePointDirection.X += FMath::RandRange(-0.5f, 0.5f);

	// MaxAngle = 45.f;
	//float rightAmountFormula = FMath::Pow(0.5f, maxNumSplinePoints - );
	float rightAmount = FMath::RandRange(-TileSize / TilesPerRow * 0.5f, TileSize / TilesPerRow * 0.5f);// FMath::RandRange(-rightAmountFormula, rightAmountFormula); // *(maxNumSplinePoints * (0.1f / maxNumSplinePoints));// FMath::RandRange(-TileSize * 0.25f, TileSize * 0.25f);
	LastSPlinePointDirection.Z += FMath::RandRange(-splineVerticalStep, splineVerticalStep);

	FVector newLocation = mainPath->GetSpline()->GetWorldLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1) + LastSPlinePointDirection * FVector{ distToNextSplinePoint, distToNextSplinePoint, 1.f} + LastSplinePointRightVector * rightAmount;
	newLocation.Z = FMath::Clamp(newLocation.Z, splineVerticalMin, splineVerticalMax);

	// Remove the first point in the spline if adding 1 exceedes the max number of spline points.
	mainPath->AddSplinePointAndMesh(newLocation);
	if (mainPath->GetSpline()->GetNumberOfSplinePoints() - 2 > maxNumSplinePoints) // -2 because the spline starts with 2 points and needs a leading point to not make gaps
		mainPath->RemoveFirstSplinePointAndMesh(true);

	lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
}

FVector AEndlessGameMode::GetTileCentreLastRow(int index)
{
	FVector previousSplinePointLoc = mainPath->GetSpline()->GetWorldLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 2);
	FVector previousSplinePointRV = mainPath->GetSpline()->GetRightVectorAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 2, ESplineCoordinateSpace::World);
	int tileOffset = FMath::Floor(TilesPerRow / 2);

	// Initial offset is half if tileOffset is even, normal if odd
	float offset = TileSize * (0.5f * ( 1 - (TilesPerRow % 2)));

	return previousSplinePointLoc + (lastSplinePointLoc - previousSplinePointLoc) * 0.5f + previousSplinePointRV * (-TileSize * tileOffset + (TileSize * index) + offset);
}

void AEndlessGameMode::SpawnPointObject(FVector& location)
{
	FActorSpawnParameters ObjectSpawnParams;
	ObjectSpawnParams.Owner = this;
	ObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APickUpActor* PointObject = GetWorld()->SpawnActor<APickUpActor>(APickUpActor::StaticClass(), FVector(), FRotator(), ObjectSpawnParams);

	// Delete the first object in the array if the array count reaches maxNumObstacles
	if (PickupActors.Num() > 0 && PickupActors.Num() >= maxNumPickups)
	{
		// Destroy the actor if it hasn't already been by the player
		if (IsValid(PickupActors[0]))
		{
			PickupActors[0]->GetMeshComponent()->UnregisterComponent();
			PickupActors[0]->Destroy();
		}
		PickupActors.RemoveAt(0);
	}

	PickupActors.Emplace(PointObject);
	
	PointObject->SetStaticMesh(PointObjectMesh);
	
	PointObject->SetActorLocation(location + FVector{ 0.f, 0.f, 50.f });
	PointObject->GetMeshComponent()->SetWorldScale3D(FVector{ 0.33f, 0.33f, 0.33f });
	PointObject->GetMeshComponent()->SetRelativeRotation(FRotator{ 90.f, 0.f, 0.f });
	//PointObject->GetMeshComponent()->RegisterComponent();
}

void AEndlessGameMode::SpawnObstacleObject(FVector& location)
{
	FActorSpawnParameters ObjectSpawnParams;
	ObjectSpawnParams.Owner = this;
	ObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AObstacleActor* ObstacleObject = GetWorld()->SpawnActor<AObstacleActor>(AObstacleActor::StaticClass(), FVector(), FRotator(), ObjectSpawnParams);

	// Delete the first object in the array if the array count reaches maxNumObstacles
	if (ObstacleActors.Num() >= maxNumObstacles)
	{
		// Destroy the actor if it hasn't already been by the player
		if (IsValid(ObstacleActors[0]))
		{
			ObstacleActors[0]->GetMeshComponent()->UnregisterComponent();
			ObstacleActors[0]->Destroy();
		}
		ObstacleActors.RemoveAt(0);
	}

	ObstacleActors.Emplace(ObstacleObject);

	int randomIndex = FMath::RandRange(0, meshPathLib.Num() - 1);
	ObstacleObject->SetStaticMesh(meshPathLib[randomIndex]);

	float tempHeight = FMath::RandRange(0.5f, 1.f);
	ObstacleObject->SetActorLocation(location); // +FVector{ 0.f, 0.f, 10.f * tempHeight });
	ObstacleObject->GetMeshComponent()->SetWorldRotation(mainPath->GetSpline()->GetRotationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World) + FRotator{ 0.f, 180.f, 0.f });
	//ObstacleObject->GetMeshComponent()->SetWorldScale3D(FVector{ FMath::RandRange(0.75f, 1.5f), FMath::RandRange(0.75f, 1.5f), FMath::RandRange(0.1f, 1.f) * tempHeight });
	//ObstacleObject->GetMeshComponent()->SetRelativeRotation(FRotator{ 0.f, FMath::RandRange(0.f, 90.f), 0.f });
	//PointObject->GetMeshComponent()->RegisterComponent();
}

void AEndlessGameMode::EndGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), "World'/Game/Maps/DefaultMap.DefaultMap'");
}

void AEndlessGameMode::OnPlayerEscapePressed()
{
	UE_LOG(LogTemp, Warning, TEXT("GameMode heard EscPressed"));
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AEndlessGameMode::OnPlayerRestartPressed()
{
	EndGame();
}
