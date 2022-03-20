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
	StartChaseBox = GetWorld()->SpawnActor<AActor>(ChaseBoxClass, mPathMaster->GetLocationAtSplinePoint(0) - FVector{ TileSize + TileSize * 0.5f, 0.f,0.f }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	StartChaseBox->SetActorScale3D(FVector{ 1.5f, (float)jObject->GetIntegerField("NumLanes") + 0.5f, 2.f });
	ChaseBoxMaxSpeed = Cast<APlayerPawn>(mainPlayer)->GetMaxSpeed();
}

void AEndlessGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the player is further than the max distance from the last point, add a new point.
	//int pathIndex = extendFromSplinePoint;
	//if (pathIndex > mPathMaster->GetNumSplinePoints() - 1)
	//	pathIndex = 0;

	// Extend if the player is at or beyond the spline point to extend from
	if (CheckShouldExtend())
	{
		// Teleport the start chase box to the new start of the spline
		FVector chaseStartDestination;
		FRotator chaseStartDestinationRot;
		mPathMaster->GetLocationAndRotationAtSplinePoint(chaseStartDestination, chaseStartDestinationRot, 2);
		StartChaseBox->SetActorRotation(chaseStartDestinationRot);
		StartChaseBox->SetActorLocation(chaseStartDestination - StartChaseBox->GetActorForwardVector() * (TileSize * 0.5f));

		mPathMaster->AddPoint(mPathMaster->GenerateNewPointLocation());
	}
	// Extend if the tracker meets or exceedes the extend rate
	else if ((mPathMaster->GetLocationAtSplineInputKey(StartChaseBoxSplineIndex) - StartChaseBox->GetActorLocation()).Size() < 10.f)
	{
		mPathMaster->AddPoint(mPathMaster->GenerateNewPointLocation());
	}

	// Move the chase box
	if(StartChaseBoxSpeed < ChaseBoxMaxSpeed)
		CalculateChaseBoxSpeed();
	FVector chaseStartDestination;
	FRotator chaseStartDestinationRot;
	mPathMaster->GetLocationAndRotationAtSplineInputKey(chaseStartDestination, chaseStartDestinationRot, StartChaseBoxSplineIndex);
	StartChaseBox->SetActorLocationAndRotation(FMath::VInterpConstantTo(StartChaseBox->GetActorLocation(), chaseStartDestination, DeltaTime, StartChaseBoxSpeed),
											   FMath::RInterpConstantTo(StartChaseBox->GetActorRotation(), chaseStartDestinationRot, DeltaTime, StartChaseBoxSpeed * 0.1f));
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
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;

	HighscoreWriter hWriter;
	FString gameModeString;
	GetGameModeStringFromInt(gameModeString, Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->mGameMode);
	bool bNewHighscore = false;

	if (hWriter.CheckShouldAddToHighscore(Cast<APlayerPawn>(mainPlayer)->pointsCollected, gameModeString))
	{
		// Store data in highscore struct
		mSlot = MakeShareable(new HighscoreSlot());
		mSlot->mDistance = FMath::Floor(Cast<APlayerPawn>(mainPlayer)->distanceTravelled * 100.f) / 100.f;
		mSlot->mScore = Cast<APlayerPawn>(mainPlayer)->pointsCollected;
		GetGameTimeString(mSlot->mTime);
		//Name will be stored in the WriteToHighscores which is called after the player has reviewed their game summary
		bNewHighscore = true;
	}

	Cast<APlayerPawn>(mainPlayer)->ShowGameSummary(bNewHighscore);
}

void AEndlessGameMode::WriteToHighscores(UPARAM(ref)FString& playerName)
{
	mSlot->mName = *playerName;

	HighscoreWriter hWriter;
	FString gameModeString;
	GetGameModeStringFromInt(gameModeString, Cast<UCustomGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->mGameMode);

	hWriter.AddToHighscore(*mSlot, gameModeString);
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

void AEndlessGameMode::GetGameTimeString(FString& returnString)
{
	const int deltatime = FMath::Floor(GetWorld()->GetTimeSeconds());

	// Minutes
	FString minutesString;
	int minutes = deltatime / 60;
	if (minutes > 9)
		minutesString = FString::FromInt(minutes);
	else
		minutesString = FString("0" + FString::FromInt(minutes));

	// Seconds
	FString secondsString;
	int seconds;
	if (deltatime > 60)
	{
		seconds = FMath::Floor(deltatime - (FMath::Floor(deltatime / 60) * 60));
		if (seconds > 9)
			secondsString = FString::FromInt(seconds);
		else
			secondsString = FString("0" + FString::FromInt(seconds));
	}
	else if (deltatime > 9)
		secondsString = FString::FromInt(deltatime);
	else
		secondsString = FString("0" + FString::FromInt(deltatime));
	
	// Milliseconds
	FString millisecondsString;
	int milliseconds = FMath::Floor((GetWorld()->GetTimeSeconds() - (float)deltatime) * 100.f);
	if (milliseconds > 9)
		millisecondsString = FString::FromInt(milliseconds);
	else
		millisecondsString = FString("0" + FString::FromInt(milliseconds));

	returnString = FString(minutesString + ":" + secondsString + ":" + millisecondsString);
}

void AEndlessGameMode::CalculateChaseBoxSpeed()
{
	StartChaseBoxSpeed = (ChaseBoxMaxSpeed - TileSize) * (1/ChaseBoxTimeToMaxSpeed) * UGameplayStatics::GetTimeSeconds(GetWorld()) + TileSize;
	StartChaseBoxSpeed = FMath::Clamp(StartChaseBoxSpeed, 0.f, ChaseBoxMaxSpeed);
	//UE_LOG(LogTemp, Warning, TEXT("StartChaseBoxSpeed: %f"), StartChaseBoxSpeed);
}

bool AEndlessGameMode::CheckShouldExtend()
{
	FVector PlayerLoc = mainPlayer->GetActorLocation();
	return mPathMaster->FindInputKeyClosestToWorldLocation(PlayerLoc) > extendFromSplinePoint;
}
