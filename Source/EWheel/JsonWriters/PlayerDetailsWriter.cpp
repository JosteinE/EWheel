// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/JsonWriters/PlayerDetailsWriter.h"
#include "PlayerDetailsWriter.h"

void UPlayerDetailsWriter::ImportFromPlayerDetailsToJson(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, FPlayerDetails& playerDetails)
{
	//FString mName = "";
	//	int mPointsCollected = 0;
	//	TArray<FVector> mBoardColours;
	//	bool bFender = false;

	jObjectWrapper.JsonObject->SetStringField(FString{ "Name" }, playerDetails.mName);
	jObjectWrapper.JsonObject->SetNumberField(FString{ "Points" }, playerDetails.mPointsCollected);
	jObjectWrapper.JsonObject->SetBoolField(FString{ "Fender" }, playerDetails.bFender);

	TArray<TSharedPtr<FJsonObject>> colours;

	colours.Init(MakeShareable(new FJsonObject()), 4);
	
	for (int i = 0; i < colours.Num(); i++)
	{
		colours[i]->SetNumberField(FString{ "R" }, playerDetails.mBoardColours[i].X);
		colours[i]->SetNumberField(FString{ "G" }, playerDetails.mBoardColours[i].Y);
		colours[i]->SetNumberField(FString{ "B" }, playerDetails.mBoardColours[i].Z);
	}
}

FPlayerDetails UPlayerDetailsWriter::ExportFromJsonToPlayerDetails(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper)
{
	return FPlayerDetails();
}
