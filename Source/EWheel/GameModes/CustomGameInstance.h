// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class EWHEEL_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(Category = GameMode, VisibleAnywhere, BlueprintReadWrite)
	int mGameMode = 0;

	// Becomes true after user settings has been applied after app startup
	UPROPERTY(Category = GameMode, VisibleAnywhere, BlueprintReadWrite)
	bool mAppConstructed = false;

	FORCEINLINE int GetGameMode() const { return mGameMode; }
};
