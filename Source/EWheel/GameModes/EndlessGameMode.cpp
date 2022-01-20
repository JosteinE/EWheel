// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/GameModes/EndlessGameMode.h"
#include "EWheel/PlayerPawn.H"
#include "EWheel/Spline/SplineActor.h"

AEndlessGameMode::AEndlessGameMode()
{
}

void AEndlessGameMode::BeginPlay()
{
	TSubclassOf<APawn> pawnClass = LoadObject<UBlueprint>(NULL, TEXT("Blueprint'/Game/Blueprints/PlayerPawnBP.PlayerPawnBP'"))->GeneratedClass;
	FActorSpawnParameters playerSpawnParams;
	playerSpawnParams.Owner = this;
	mainPlayer = GetWorld()->SpawnActor<APawn>(pawnClass, FVector{ 0,0,0 }, FRotator{ 0,0,0 }, playerSpawnParams);
	GetWorld()->GetFirstPlayerController()->Possess(mainPlayer);

	FActorSpawnParameters pathSpawnParams;
	pathSpawnParams.Owner = this;
	mainPath = GetWorld()->SpawnActor<ASplineActor>(ASplineActor::StaticClass(), FVector{ 0.f, 0.f, -2.f }, FRotator{ 0.f, 0.f, 0.f }, pathSpawnParams);
}

void AEndlessGameMode::EndGame()
{
}
