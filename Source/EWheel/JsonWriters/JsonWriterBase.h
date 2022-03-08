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

public:

	UJsonWriterBase();
	~UJsonWriterBase();

	// Using UPARAM(ref) because without the struct parameter becomes a return value for some reason. 
	//Json Set Functions
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

	//Json Get Functions
	UFUNCTION(BlueprintCallable)
	static bool GetJsonBoolField(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fieldName);

	UFUNCTION(BlueprintCallable)
	static float GetJsonNumberField(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fieldName);

	UFUNCTION(BlueprintCallable)
	static FString GetJsonStringField(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fieldName);

	UFUNCTION(BlueprintCallable)
	static bool GetJsonObjectField(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fieldName, FJsonObjectWrapper& jsonObjectWrapper);

	// Read/Write Functions
	UFUNCTION(BlueprintCallable)
	static void WriteJsonToFile(UPARAM(ref) FJsonObjectWrapper& jObjectWrapper, const FString& fileName);

	UFUNCTION(BlueprintCallable)
	static bool LoadJsonFileToWrapper(FJsonObjectWrapper& NewJsonWrapper, const FString& fileName);
private:

	template <class T>
	TSharedPtr<FJsonValue> ToJsonValue(T inValue);
};
