// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EndlessGameMode.generated.h"
/**
 * 
 */
class ASplineActor;
UCLASS()
class EWHEEL_API AEndlessGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AEndlessGameMode();

private:
	void BeginPlay() override;

public:
	void EndGame();

private:
	APawn* mainPlayer;
	ASplineActor* mainPath;
	APlayerController* mainPlayerController;
};
