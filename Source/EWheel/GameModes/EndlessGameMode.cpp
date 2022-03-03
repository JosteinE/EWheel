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

#include "EWheel/JsonWriters/HighscoreWriter.h"
#include "EWheel/GameModes/CustomGameInstance.h"

AEndlessGameMode::AEndlessGameMode()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	// Get the path's default material
	static ConstructorHelpers::FObjectFinder<UMaterialInterface>DefaultMaterialAsset(TEXT("Material'/Game/Materials/GrassDirtMasked_Material.GrassDirtMasked_Material'"));
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

	static ConstructorHelpers::FObjectFinder<UClass>ChaseBoxAsset(TEXT("Blueprint'/Game/Blueprints/PlayerChaseBoxBP.PlayerChaseBoxBP_C'"));
	if (ChaseBoxAsset.Object)
		ChaseBoxClass = ChaseBoxAsset.Object;
}

void AEndlessGameMode::BeginPlay()
{
	// Set The Game Mode
	mGameMode = Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->mGameMode;
	UE_LOG(LogTemp, Warning, TEXT("GameMode: %i"), mGameMode);
	//UE_LOG(LogTemp, Warning, TEXT("IntermDir: %s"), *FPaths::ProjectIntermediateDir());
	//UE_LOG(LogTemp, Warning, TEXT("LaunchDir: %s"), *FPaths::LaunchDir());


	// Get the player
	mainPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();

	//Bind Delegates
	Cast<APlayerPawn>(mainPlayer)->EscPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerEscapePressed);
	Cast<APlayerPawn>(mainPlayer)->RestartPressed.AddDynamic(this, &AEndlessGameMode::OnPlayerRestartPressed);
	Cast<APlayerPawn>(mainPlayer)->PlayerDeath.AddDynamic(this, &AEndlessGameMode::OnPlayerDeath);

	// Spawn the path
	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	pathSpawnParams.Owner = this;
	mPathMaster = GetWorld()->SpawnActor<AMeshSplineMaster>(AMeshSplineMaster::StaticClass(), FVector{ 0.f, 0.f, 0.f }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	mPathMaster->SetDefaultMaterial(DefaultMaterial);
	mPathMaster->SetMaxNumSplinePoints(maxNumSplinePoints);
	mPathMaster->SetTileSize(TileSize);
	mPathMaster->SetUseHighResModels(false);

	// Get the user user defined values to construct the path
	FString modeString;
	GetGameModeStringFromInt(modeString, mGameMode);
	FString jString;
	const FString jFilePath = FPaths::LaunchDir() + modeString + "EndlessSettings.json"; //FPaths::ProjectIntermediateDir()
	FFileHelper::LoadFileToString(jString, *jFilePath);
	TSharedPtr<FJsonObject> jObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> jReader = TJsonReaderFactory<>::Create(jString);


	if (!jObject.IsValid() || !FJsonSerializer::Deserialize(jReader, jObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("couldn't deserialize"));
		UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), TEnumAsByte<EQuitPreference::Type>(EQuitPreference::Quit), true);
	}
	// Construct the desired numbers of lanes
	mPathMaster->ConstructSplines(jObject->GetIntegerField("NumLanes"));

	// Create an empty starting area
	mPathMaster->SetSpawnPits(false);
	mPathMaster->SetSpawnRamps(false);
	mPathMaster->SetSpawnHoles(false);
	mPathMaster->SetObstacleSpawnChance(0);
	mPathMaster->SetPointSpawnChance(0);
	mPathMaster->SetPowerUpSpawnChance(0);

	// Create the first few safe rows
	for (int i = 0; i < 3; i++)
	{
		mPathMaster->AddPoint(mPathMaster->GenerateNewPointLocation());
	}

	// Set default path properties
	mPathMaster->LoadFromJson(jObject);

	// Finish making the path
	while (!mPathMaster->GetIsAtMaxSplinePoints())
	{
		mPathMaster->AddPoint(mPathMaster->GenerateNewPointLocation());
	}

	// SpawnChaseBox
	ChaseBoxStart = GetWorld()->SpawnActor<AActor>(ChaseBoxClass, mPathMaster->GetLocationAtSplinePoint(0) - FVector{ (float)TileSize * 0.5f, 0.f, 0.f }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	ChaseBoxStart->SetActorScale3D(FVector{ 1.5f, (float)jObject->GetIntegerField("NumLanes") + 0.5f, 2.f });
}

void AEndlessGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the player is further than the max distance from the last point, add a new point.
	//int pathIndex = extendFromSplinePoint;
	//if (pathIndex > mPathMaster->GetNumSplinePoints() - 1)
	//	pathIndex = 0;
	extendRateTracker += DeltaTime;

	if(CheckShouldExtend())
	{
		// Teleport the start chase box to the new start of the spline
		FVector chaseStartDestination;
		FRotator chaseStartDestinationRot;
		mPathMaster->GetLocationAndRotationAtSplinePoint(chaseStartDestination, chaseStartDestinationRot, 2);
		ChaseBoxStart->SetActorRotation(chaseStartDestinationRot);
		ChaseBoxStart->SetActorLocation(chaseStartDestination - ChaseBoxStart->GetActorForwardVector() * (TileSize * 0.5f));

		mPathMaster->AddPoint(mPathMaster->GenerateNewPointLocation());
		ChaseBoxStartSplineIndex = 2;
		extendRateTracker = 0.f;
	}
	else if(extendRateTracker >= pathExtendRate)
	{
		mPathMaster->AddPoint(mPathMaster->GenerateNewPointLocation());
		ChaseBoxStartSplineIndex--;
		extendRateTracker = 0.f;
	}

	// Chase box
	CalculateExtendRate(DeltaTime);
	CalculateChaseBoxSpeed();
	CalculateChaseBoxIndex();
	FVector chaseStartDestination;
	FRotator chaseStartDestinationRot;
	float currentSpeed = Cast<APlayerPawn>(mainPlayer)->GetCurrentSpeed();
	mPathMaster->GetLocationAndRotationAtSplinePoint(chaseStartDestination, chaseStartDestinationRot, ChaseBoxStartSplineIndex);
	ChaseBoxStart->SetActorLocationAndRotation(FMath::VInterpConstantTo(ChaseBoxStart->GetActorLocation(), chaseStartDestination, DeltaTime, ChaseBoxStartSpeed),
											   FMath::RInterpConstantTo(ChaseBoxStart->GetActorRotation(), chaseStartDestinationRot, DeltaTime, ChaseBoxStartSpeed * 0.1));
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

void AEndlessGameMode::GetGameModeStringFromInt(FString& returnString, int mode)
{
	switch (mode)
	{
	case 0:
		returnString = FString{ "Easy" };
		break;
	case 1:
		returnString = FString{ "Medium" };
		break;
	case 2:
		returnString = FString{ "Hard" };
		break;
	case 3:
		returnString = FString{ "Custom" };
		break;
	default:
		break;
	}
}

void AEndlessGameMode::CalculateExtendRate(float DeltaTime)
{
	float maxSpeed = TileSize / Cast<APlayerPawn>(mainPlayer)->GetMaxSpeed();
	pathExtendRate = FMath::Clamp(pathExtendRate - ((1.f - maxSpeed) /60.f) * DeltaTime, maxSpeed, pathExtendRate);
	UE_LOG(LogTemp, Warning, TEXT("pathExtendRate: %f"), pathExtendRate);
}

void AEndlessGameMode::CalculateChaseBoxIndex()
{
	if ((mPathMaster->GetLocationAtSplinePoint(ChaseBoxStartSplineIndex) - ChaseBoxStart->GetActorLocation()).Size() < ChaseBoxDistanceThreshold)
		ChaseBoxStartSplineIndex++;
}

void AEndlessGameMode::CalculateChaseBoxSpeed()
{
	float maxSpeed = Cast<APlayerPawn>(mainPlayer)->GetMaxSpeed();
	ChaseBoxStartSpeed = maxSpeed * (2 - pathExtendRate) - (maxSpeed - TileSize);
	ChaseBoxStartSpeed = FMath::Clamp(ChaseBoxStartSpeed, 0.f, maxSpeed);
	UE_LOG(LogTemp, Warning, TEXT("ChaseBoxStartSpeed: %f"), ChaseBoxStartSpeed);
}

bool AEndlessGameMode::CheckShouldExtend()
{
	FVector PlayerLoc = mainPlayer->GetActorLocation();
	if (mPathMaster->FindInputKeyClosestToWorldLocation(PlayerLoc) > extendFromSplinePoint)
		return true;
	return false;
}
