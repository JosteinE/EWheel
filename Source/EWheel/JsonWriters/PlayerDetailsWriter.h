// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonObjectWrapper.h"
#include "EWheel/JsonWriters/PlayerDetails.h"
#include "PlayerDetailsWriter.generated.h"


/**
 * 
 */
UCLASS()
class EWHEEL_API UPlayerDetailsWriter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	static void SetPlayerName(const FString& playerName);

	UFUNCTION(BlueprintCallable)
	static void SetPlayerPoints(int points);

	UFUNCTION(BlueprintCallable)
	static void SetPlayerFenderEnabled(bool bFender);

	UFUNCTION(BlueprintCallable)
	static void SetPlayerSkinSelected(int skinIndex);

	UFUNCTION(BlueprintCallable)
	static void SetPlayerSkinUnlocked(int index, bool bUnlocked);

	UFUNCTION(BlueprintCallable)
	static void SetPlayerColours(UPARAM(ref)TArray<FVector>& colours);

	UFUNCTION(BlueprintCallable)
	static void WriteFromPlayerDetailsToJson(UPARAM(ref)FPlayerDetails& playerDetails);

	UFUNCTION(BlueprintCallable)
	static FPlayerDetails& LoadFromJsonToPlayerDetails(UPARAM(ref)FPlayerDetails& playerDetails);
};
