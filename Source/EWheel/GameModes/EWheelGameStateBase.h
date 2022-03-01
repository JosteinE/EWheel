// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EWheelGameStateBase.generated.h"

UENUM()
enum GAMEMODE
{
	EASY,
	MEDIUM,
	HARD,
	CUSTOM
};

UCLASS()
class EWHEEL_API AEWheelGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
	UPROPERTY(Category = GameMode, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int GameMode = 0;
};
