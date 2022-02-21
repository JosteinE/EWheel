// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/GameModes/EndlessGameMode.h"
#include "EWheel/PlayerPawn.h"
//#include "EWheel/Spline/MeshSplineActor.h"
#include "EWheel/Spline/MeshSplineMaster.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

#include "EWheel/Objects/ObstacleActor.h"
#include "EWheel/Objects/PickUpActor.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/HUD.h"

AEndlessGameMode::AEndlessGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>PointObjectAsset(TEXT("StaticMesh'/Game/Meshes/PointObject.PointObject'"));
	if (PointObjectAsset.Succeeded())
		PointObjectMesh = PointObjectAsset.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh>ObstacleObjectAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (ObstacleObjectAsset.Succeeded())
		ObstacleMesh = ObstacleObjectAsset.Object;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>DefaultMaterialAsset(TEXT("Material'/Game/Materials/GrassDirt_Material.GrassDirt_Material'"));
	if (DefaultMaterialAsset.Succeeded())
		DefaultMaterial = DefaultMaterialAsset.Object;

	//UObject* HudAsset = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Blueprint'/Game/Blueprints/PlayerHud.PlayerHUD'"));
	//if (HudAsset)
	//	this->HUDClass = Cast<UBlueprint>(HudAsset)->GeneratedClass;

	static ConstructorHelpers::FObjectFinder<UClass>HudAsset(TEXT("Blueprint'/Game/Blueprints/PlayerHud.PlayerHUD_C'"));
	if (HudAsset.Object)
		this->HUDClass = HudAsset.Object;

	this->bStartPlayersAsSpectators = false;
	static ConstructorHelpers::FObjectFinder<UClass>PawnAsset(TEXT("Blueprint'/Game/Blueprints/PlayerPawnBP.PlayerPawnBP_C'"));
	if (PawnAsset.Object)
		this->DefaultPawnClass = PawnAsset.Object;
}

void AEndlessGameMode::BeginPlay()
{
	// Spawn the path
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;
	mPathMaster = GetWorld()->SpawnActor<AMeshSplineMaster>(AMeshSplineMaster::StaticClass(), FVector{ 0.f, 0.f, 0.f }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	mPathMaster->SetDefaultMaterial(DefaultMaterial);
	mPathMaster->SetMaxNumSplinePoints(maxNumSplinePoints);
	mPathMaster->SetTileSize(TileSize);
	mPathMaster->ConstructSplines(mNumSplines);
	mPathMaster->SetUseHighResModels(false);

	// Create an empty starting area
	mPathMaster->SetSpawnPits(false);
	mPathMaster->SetSpawnRamps(false);
	mPathMaster->SetSpawnHoles(false);
	mPathMaster->SetObstacleSpawnChance(0);
	mPathMaster->SetPointSpawnChance(0);
	mPathMaster->SetPowerUpSpawnChance(0);

	// Create the first few rows
	for (int i = 0; i < 3; i++)
	{
		ExtendPath();
	}

	mPathMaster->SetSpawnPits(true);
	mPathMaster->SetSpawnRamps(true);
	mPathMaster->SetSpawnHoles(true); // Should only be enabled once jump is acquired
	mPathMaster->SetObstacleSpawnChance(33);
	mPathMaster->SetPointSpawnChance(8);
	mPathMaster->SetPowerUpSpawnChance(0);

	// Spawn the player
	//FActorSpawnParameters playerSpawnParams;
	//playerSpawnParams.Owner = this;
	//playerSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//mainPlayer = GetWorld()->SpawnActor<APlayerPawn>(pawnClass, FVector{ 150.f,0, playerSpawnHeight }, FRotator{ 0,0,0 }, playerSpawnParams);
	//GetWorld()->GetFirstPlayerController()->Possess(mainPlayer);

	mainPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();

	//Bind Delegates
	Cast<APlayerPawn>(mainPlayer)->EscPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerEscapePressed);
	Cast<APlayerPawn>(mainPlayer)->RestartPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerRestartPressed);
	Cast<APlayerPawn>(mainPlayer)->PlayerDeath.AddDynamic(this, &AEndlessGameMode::OnPlayerDeath);

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
	if (pathIndex > mPathMaster->GetNumSplinePoints() - 1)
		pathIndex = 0;

	// Extend the path if the player is within the minimum range to indexed spline point
	if (mPathMaster->GetIsAtMaxSplinePoints() || (mPathMaster->GetLocationAtSplinePoint(pathIndex) - mainPlayer->GetActorLocation()).Size() < minDistToSplinePoint)
	{
		ExtendPath();
	}
}

void AEndlessGameMode::ExtendPath()
{
	mPathMaster->AddPoint(mPathMaster->GenerateNewPointLocation());

	//// Calculate the next spline point position
	//FVector LastSPlinePointDirection = mainPath->GetSpline()->GetDirectionAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	//FVector LastSplinePointRightVector = mainPath->GetSpline()->GetRightVectorAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	////LastSPlinePointDirection.X += FMath::RandRange(-0.5f, 0.5f);

	//// MaxAngle = 45.f;
	////float rightAmountFormula = FMath::Pow(0.5f, maxNumSplinePoints - );
	//float rightAmount = FMath::RandRange(-TileSize / TilesPerRow * 0.5f, TileSize / TilesPerRow * 0.5f);// FMath::RandRange(-rightAmountFormula, rightAmountFormula); // *(maxNumSplinePoints * (0.1f / maxNumSplinePoints));// FMath::RandRange(-TileSize * 0.25f, TileSize * 0.25f);
	//LastSPlinePointDirection.Z += FMath::RandRange(-splineVerticalStep, splineVerticalStep);

	//FVector newLocation = mainPath->GetSpline()->GetWorldLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1) + LastSPlinePointDirection * FVector{ distToNextSplinePoint, distToNextSplinePoint, 1.f} + LastSplinePointRightVector * rightAmount;
	//newLocation.Z = FMath::Clamp(newLocation.Z, splineVerticalMin, splineVerticalMax);

	//// Remove the first point in the spline if adding 1 exceedes the max number of spline points.
	//mainPath->AddSplinePointAndMesh(newLocation);
	//if (mainPath->GetSpline()->GetNumberOfSplinePoints() - 2 > maxNumSplinePoints) // -2 because the spline starts with 2 points and needs a leading point to not make gaps
	//	mainPath->RemoveFirstSplinePointAndMesh(true);

	//lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
}

FVector AEndlessGameMode::GetTileCentreLastRow(int index)
{
	//FVector previousSplinePointLoc = mainPath->GetSpline()->GetWorldLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 2);
	//FVector previousSplinePointRV = mainPath->GetSpline()->GetRightVectorAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 2, ESplineCoordinateSpace::World);
	//int tileOffset = FMath::Floor(TilesPerRow / 2);

	//// Initial offset is half if tileOffset is even, normal if odd
	//float offset = TileSize * (0.5f * ( 1 - (TilesPerRow % 2)));

	//return previousSplinePointLoc + (lastSplinePointLoc - previousSplinePointLoc) * 0.5f + previousSplinePointRV * (-TileSize * tileOffset + (TileSize * index) + offset);
	return FVector{};
}

void AEndlessGameMode::SpawnPointObject(FVector& location)
{
	//FActorSpawnParameters ObjectSpawnParams;
	//ObjectSpawnParams.Owner = this;
	//ObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//APickUpActor* PointObject = GetWorld()->SpawnActor<APickUpActor>(APickUpActor::StaticClass(), FVector(), FRotator(), ObjectSpawnParams);

	//// Delete the first object in the array if the array count reaches maxNumObstacles
	//if (PickupActors.Num() > 0 && PickupActors.Num() >= maxNumPickups)
	//{
	//	// Destroy the actor if it hasn't already been by the player
	//	if (IsValid(PickupActors[0]))
	//	{
	//		PickupActors[0]->GetMeshComponent()->UnregisterComponent();
	//		PickupActors[0]->Destroy();
	//	}
	//	PickupActors.RemoveAt(0);
	//}

	//PickupActors.Emplace(PointObject);
	//
	//PointObject->SetStaticMesh(PointObjectMesh);
	//
	//PointObject->SetActorLocation(location + FVector{ 0.f, 0.f, 50.f });
	//PointObject->GetMeshComponent()->SetWorldScale3D(FVector{ 0.33f, 0.33f, 0.33f });
	//PointObject->GetMeshComponent()->SetRelativeRotation(FRotator{ 90.f, 0.f, 0.f });
	////PointObject->GetMeshComponent()->RegisterComponent();
}

void AEndlessGameMode::SpawnObstacleObject(FVector& location)
{
	//FActorSpawnParameters ObjectSpawnParams;
	//ObjectSpawnParams.Owner = this;
	//ObjectSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//AObstacleActor* ObstacleObject = GetWorld()->SpawnActor<AObstacleActor>(AObstacleActor::StaticClass(), FVector(), FRotator(), ObjectSpawnParams);

	//// Delete the first object in the array if the array count reaches maxNumObstacles
	//if (ObstacleActors.Num() >= maxNumObstacles)
	//{
	//	// Destroy the actor if it hasn't already been by the player
	//	if (IsValid(ObstacleActors[0]))
	//	{
	//		ObstacleActors[0]->GetMeshComponent()->UnregisterComponent();
	//		ObstacleActors[0]->Destroy();
	//	}
	//	ObstacleActors.RemoveAt(0);
	//}

	//ObstacleActors.Emplace(ObstacleObject);

	//int randomIndex = FMath::RandRange(0, meshPathLib.Num() - 1);
	//ObstacleObject->SetStaticMesh(meshPathLib[randomIndex]);

	//float tempHeight = FMath::RandRange(0.5f, 1.f);
	//ObstacleObject->SetActorLocation(location); // +FVector{ 0.f, 0.f, 10.f * tempHeight });
	//ObstacleObject->GetMeshComponent()->SetWorldRotation(mainPath->GetSpline()->GetRotationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World) + FRotator{ 0.f, 180.f, 0.f });
	////ObstacleObject->GetMeshComponent()->SetWorldScale3D(FVector{ FMath::RandRange(0.75f, 1.5f), FMath::RandRange(0.75f, 1.5f), FMath::RandRange(0.1f, 1.f) * tempHeight });
	////ObstacleObject->GetMeshComponent()->SetRelativeRotation(FRotator{ 0.f, FMath::RandRange(0.f, 90.f), 0.f });
	////PointObject->GetMeshComponent()->RegisterComponent();
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

void AEndlessGameMode::OnPlayerDeath()
{
}
