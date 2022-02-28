// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonObjectWrapper.h"
#include "JsonWriterBase.generated.h"

/**
 *
 */

UCLASS()
class EWHEEL_API UJsonWriterBase : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UJsonWriterBase();
	~UJsonWriterBase();

public:
	// Using UPARAM(ref) because without the struct parameter becomes a return value for some reason. 
	UFUNCTION(BlueprintCallable)
	static void InitJsonObject(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper);

	UFUNCTION(BlueprintCallable)
	static void SetJsonBoolField(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fieldName, bool value);

	UFUNCTION(BlueprintCallable)
	static void SetJsonNumberField(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fieldName, float value);

	UFUNCTION(BlueprintCallable)
	static void SetJsonStringField(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fieldName, const FString& value);

	UFUNCTION(BlueprintCallable)
	static void SetJsonObjectField(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fieldName, UPARAM(ref) FJsonObjectWrapper& newJObjectWrapper);

	UFUNCTION(BlueprintCallable)
	static void WriteJsonToFile(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fileName);
private:

	template <class T>
	TSharedPtr<FJsonValue> ToJsonValue(T inValue);
};
