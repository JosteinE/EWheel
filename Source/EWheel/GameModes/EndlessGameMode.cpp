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

	this->HUDClass = LoadObject<UBlueprint>(NULL, TEXT("Blueprint'/Game/Blueprints/PlayerHud.PlayerHUD'"))->GeneratedClass;
}

void AEndlessGameMode::BeginPlay()
{
	// Spawn the player
	auto pawnClass = LoadObject<UBlueprint>(NULL, TEXT("Blueprint'/Game/Blueprints/PlayerPawnBP.PlayerPawnBP'"))->GeneratedClass;
	FActorSpawnParameters playerSpawnParams;
	playerSpawnParams.Owner = this;
	playerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	mainPlayer = GetWorld()->SpawnActor<APlayerPawn>(pawnClass, FVector{ 0,0, playerSpawnHeight }, FRotator{ 0,0,0 }, playerSpawnParams);
	GetWorld()->GetFirstPlayerController()->Possess(mainPlayer);

	//Bind Delegates
	Cast<APlayerPawn>(mainPlayer)->EscPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerEscapePressed);
	Cast<APlayerPawn>(mainPlayer)->RestartPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerRestartPressed);

	// Spawn the path
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;
	mainPath = GetWorld()->SpawnActor<AMeshSplineActor>(AMeshSplineActor::StaticClass(), FVector{ -50.f, 0.f, splineSpawnVerticalOffset }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);

	//TEST
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/DefaultGround_150x150_Sub.DefaultGround_150x150_Sub'");
	// Pits
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_150x150.Ground_Pit_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_4W_150x150.Ground_Pit_4W_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_EndP_SN_150x150.Ground_Pit_EndP_SN_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_L_SNW_150x150.Ground_Pit_L_SNW_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_T_SN_150x150.Ground_Pit_T_SN_150x150'");
	// Ramps
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_N_150x150.Ground_Ramp_N_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_NE_150x150.Ground_Ramp_NE_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_NW_150x150.Ground_Ramp_NW_150x150'");
	meshPathLib.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_Single_150x150.Ground_Ramp_Single_150x150'");


}

void AEndlessGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the player is further than the max distance from the last point, add a new point.
	int pathIndex = extendFromSplinePoint;
	if (pathIndex > mainPath->GetSpline()->GetNumberOfSplinePoints() - 1)
		pathIndex = mainPath->GetSpline()->GetNumberOfSplinePoints() - 1;

	// Extend the path if the player is within the minimum range to indexed spline point
	if ((mainPath->GetSpline()->GetWorldLocationAtSplinePoint(pathIndex) - mainPlayer->GetActorLocation()).Size() < minDistToSplinePoint)
	{
		ExtendPath();

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
			else if (FMath::RandRange(0, 99) < ObstacleSpawnChance)
			{
				FVector tileCentre = GetTileCentreLastRow(i);
				SpawnObstacleObject(tileCentre);
			}
		}
	}
}

void AEndlessGameMode::ExtendPath()
{
	// Randomly select tiles
	TArray<FString> meshPaths;
	for (int i = 0; i < TilesPerRow; i++)
	{
		meshPaths.Emplace(meshPathLib[2]); //meshPathLib[FMath::RandRange(0, meshPathLib.Num() - 1)]
	}
	mainPath->SetDefaultMesh(meshGen.GenerateStaticMeshFromTile(meshPaths));

	// Calculate the next spline point position
	FVector LastSPlinePointDirection = mainPath->GetSpline()->GetDirectionAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	//LastSPlinePointDirection.X += FMath::RandRange(-0.5f, 0.5f);
	LastSPlinePointDirection.Y += FMath::RandRange(-0.5f, 0.5f);
	LastSPlinePointDirection.Z = 0; // FMath::RandRange(-50.f, 50.f);

	const FVector newLocation = mainPath->GetSpline()->GetWorldLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1) * FVector{ 1.f, 1.f, 0.f } + LastSPlinePointDirection * FVector{ distToNextSplinePoint, distToNextSplinePoint, 1 };


	// Remove the first point in the spline if adding 1 exceedes the max number of spline points.
	mainPath->AddSplinePointAndMesh(newLocation);
	if (mainPath->GetSpline()->GetNumberOfSplinePoints() > maxNumSplinePoints)
		mainPath->RemoveFirstSplinePointAndMesh(true);

	lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
}

FVector AEndlessGameMode::GetTileCentreLastRow(int index)
{
	FVector previousSplinePointLoc = mainPath->GetSpline()->GetWorldLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 2);
	FVector previousSplinePointRV = mainPath->GetSpline()->GetRightVectorAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 2, ESplineCoordinateSpace::World);
	unsigned int tileOffset = FMath::Floor(TilesPerRow / 2);

	return previousSplinePointLoc + (lastSplinePointLoc - previousSplinePointLoc) * 0.5f + previousSplinePointRV * (-150.f * tileOffset + (150.f * index));
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

	ObstacleObject->SetStaticMesh(ObstacleMesh);

	ObstacleObject->SetActorLocation(location + FVector{ 0.f, 0.f, 17.f });
	ObstacleObject->GetMeshComponent()->SetWorldScale3D(FVector{ FMath::RandRange(0.5f, 1.f), FMath::RandRange(0.5f, 1.f), FMath::RandRange(0.5f, 1.f) });
	//ObstacleObject->GetMeshComponent()->SetRelativeRotation(FRotator{ 90.f, 0.f, 0.f });
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
