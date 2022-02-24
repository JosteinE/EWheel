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

#include "Dom/JsonObject.h"

AEndlessGameMode::AEndlessGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	// Get the path's default material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>DefaultMaterialAsset(TEXT("Material'/Game/Materials/GrassDirt_Material.GrassDirt_Material'"));
	if (DefaultMaterialAsset.Succeeded())
		DefaultMaterial = DefaultMaterialAsset.Object;

	// Get the player's HUD class
	static ConstructorHelpers::FObjectFinder<UClass>HudAsset(TEXT("Blueprint'/Game/Blueprints/PlayerHud.PlayerHUD_C'"));
	if (HudAsset.Object)
		this->HUDClass = HudAsset.Object;

	// Get the player class
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
	mPathMaster->SetUseHighResModels(false);
	mPathMaster->ConstructSplines(mNumSplines);

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
		mPathMaster->AddPoint(mPathMaster->GenerateNewPointLocation());
	}

	// Set default path values
	SetPathValuesFromFile();
	//mPathMaster->SetSpawnPits(true);
	//mPathMaster->SetSpawnRamps(true);
	//mPathMaster->SetSpawnHoles(true); // Should only be enabled once jump is acquired
	//mPathMaster->SetObstacleSpawnChance(33);
	//mPathMaster->SetPointSpawnChance(8);
	//mPathMaster->SetPowerUpSpawnChance(0);

	// Get the player
	mainPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();

	//Bind Delegates
	Cast<APlayerPawn>(mainPlayer)->EscPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerEscapePressed);
	Cast<APlayerPawn>(mainPlayer)->RestartPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerRestartPressed);
	Cast<APlayerPawn>(mainPlayer)->PlayerDeath.AddDynamic(this, &AEndlessGameMode::OnPlayerDeath);
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
		mPathMaster->AddPoint(mPathMaster->GenerateNewPointLocation());
	}
}

void AEndlessGameMode::SetPathValuesFromFile()
{
	// The code below was inspired by Orfeas, at https://www.orfeasel.com/parsing-json-files/
	FString jsonString;
	const FString JsonFilePath = FPaths::ProjectContentDir() + "/DataTables/" + "CustomEndless.json";
	FFileHelper::LoadFileToString(jsonString, *JsonFilePath);

	//Create a json object to store the information from the json string
	//The json reader is used to deserialize the json object later on
	TSharedPtr<FJsonObject> jasonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(jsonString);

	if (FJsonSerializer::Deserialize(jsonReader, jasonObject) && jasonObject.IsValid())
	{
		mPathMaster->LoadFromJson(jasonObject);
		mPathMaster->SetSpawnPits(jasonObject->GetBoolField("PitsEnabled"));
		mPathMaster->SetSpawnRamps(jasonObject->GetBoolField("RampsEnabled"));
		mPathMaster->SetSpawnHoles(jasonObject->GetBoolField("HolesEnabled")); // Should only be enabled once jump is acquired
		mPathMaster->SetObstacleSpawnChance(jasonObject->GetIntegerField("ObstacleSpawnChance"));
		mPathMaster->SetPointSpawnChance(jasonObject->GetIntegerField("PointSpawnChance"));
		mPathMaster->SetPowerUpSpawnChance(jasonObject->GetIntegerField("PowerupSpawnChance"));
	}
	else
	{
		GLog->Log("couldn't deserialize");
	}
}

void AEndlessGameMode::EndGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), GetWorld()->GetFName());
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
