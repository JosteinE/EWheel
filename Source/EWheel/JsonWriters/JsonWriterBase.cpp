// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/JsonWriters/JsonWriterBase.h"

UJsonWriterBase::UJsonWriterBase()
{
}

UJsonWriterBase::~UJsonWriterBase()
{
}

void UJsonWriterBase::InitJsonObject(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper)
{
	jObjectWrapper.JsonObject = MakeShareable(new FJsonObject());
}

void UJsonWriterBase::SetJsonBoolField(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, const FString& fieldName, bool value)
{
	jObjectWrapper.JsonObject->SetBoolField(fieldName, value);
}

void UJsonWriterBase::SetJsonNumberField(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, const FString& fieldName, float value)
{
	jObjectWrapper.JsonObject->SetNumberField(fieldName, value);
}

void UJsonWriterBase::SetJsonStringField(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, const FString& fieldName, const FString& value)
{
	jObjectWrapper.JsonObject->SetStringField(fieldName, value);
}

void UJsonWriterBase::SetJsonObjectField(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, const FString& fieldName, UPARAM(ref) FJsonObjectWrapper& newJObjectWrapper)
{
	jObjectWrapper.JsonObject->SetObjectField(fieldName, newJObjectWrapper.JsonObject);
}

void UJsonWriterBase::WriteJsonToFile(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, const FString& fileName)
{
	// Thanks to Wraiyth for guidance on how to serialize the json http://www.wraiyth.com/?p=198
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(jObjectWrapper.JsonObject.ToSharedRef(), Writer);
	
	FString jFilePath = FPaths::ProjectIntermediateDir() + fileName + ".json";

	FFileHelper FileHelper;
	if (FileHelper.SaveStringToFile(OutputString, *jFilePath))
		UE_LOG(LogTemp, Warning, TEXT("Saved successfully to file"))
	else
		UE_LOG(LogTemp, Warning, TEXT("Failed to save to file"))
}

template <class T>
TSharedPtr<FJsonValue> UJsonWriterBase::ToJsonValue(T inValue)
{
	TSharedPtr<FJsonValue> jValue = MakeShareable(new FJsonValue(inValue));
	return jValue;
}