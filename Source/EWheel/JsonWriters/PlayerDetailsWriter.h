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
	static void ImportFromPlayerDetailsToJson(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, FPlayerDetails& playerDetails);

	UFUNCTION(BlueprintCallable)
	static FPlayerDetails ExportFromJsonToPlayerDetails(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper);
};
