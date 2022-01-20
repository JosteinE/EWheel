// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/GameModes/EndlessGameMode.h"
#include "EWheel/PlayerPawn.H"
#include "EWheel/Spline/SplineActor.h"

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
	mainPath = GetWorld()->SpawnActor<ASplineActor>(ASplineActor::StaticClass(), FVector{ 0.f, 0.f, splineSpawnVerticalOffset }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
	lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
}

void AEndlessGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the player is further than the max distance from the last point, add a new point.
	if ((lastSplinePointLoc - mainPlayer->GetActorLocation()).Size() < minDistToLastSplinePoint)
	{
		const FVector LastSPlinePointDirection = mainPath->GetSpline()->GetDirectionAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
		const FVector newLocation = mainPlayer->GetActorLocation() + mainPlayer->GetActorForwardVector() * FVector { distToNextSplinePoint, distToNextSplinePoint, 0 };

		// Remove the first point in the spline if it exceedes the max number of spline points.
		if (mainPath->GetSpline()->GetNumberOfSplinePoints() + 1 > maxNumSplinePoints)
		{
			mainPath->AddSplinePoint(newLocation, false);
			mainPath->RemoveFirstSplinePoint(true);
		}
		else
			mainPath->AddSplinePoint(newLocation, true);

		lastSplinePointLoc = mainPath->GetSpline()->GetLocationAtSplinePoint(mainPath->GetSpline()->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	}
}



void AEndlessGameMode::EndGame()
{
}
