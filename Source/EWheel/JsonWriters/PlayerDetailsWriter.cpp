// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/JsonWriters/PlayerDetailsWriter.h"
#include "EWheel/JsonWriters/JsonWriterBase.h"

void UPlayerDetailsWriter::SetPlayerName(const FString& playerName)
{
	FPlayerDetails playerDetails = LoadFromJsonToPlayerDetails();
	playerDetails.mName = playerName;
	WriteFromPlayerDetailsToJson(playerDetails);
}

void UPlayerDetailsWriter::SetPlayerPoints(int points)
{
	FPlayerDetails playerDetails = LoadFromJsonToPlayerDetails();
	playerDetails.mPointsCollected = points;
	WriteFromPlayerDetailsToJson(playerDetails);
}

void UPlayerDetailsWriter::SetPlayerFenderEnabled(bool bFender)
{
	FPlayerDetails playerDetails = LoadFromJsonToPlayerDetails();
	playerDetails.bFender = bFender;
	WriteFromPlayerDetailsToJson(playerDetails);
}

void UPlayerDetailsWriter::SetPlayerSkinUnlocked(int index, bool bUnlocked)
{
	FPlayerDetails playerDetails = LoadFromJsonToPlayerDetails();
	if (index < playerDetails.mSkinsUnlocked.Num())
	{
		playerDetails.mSkinsUnlocked[index] = bUnlocked;
		WriteFromPlayerDetailsToJson(playerDetails);
	}
}

void UPlayerDetailsWriter::SetPlayerColours(UPARAM(ref)TArray<FVector>& colours)
{
	FPlayerDetails playerDetails = LoadFromJsonToPlayerDetails();

	if (playerDetails.mBoardColours.Num() != colours.Num())
		return;

	for (int i = 0; i < playerDetails.mBoardColours.Num(); i++)
	{
		playerDetails.mBoardColours[i] = colours[i];
	}

	WriteFromPlayerDetailsToJson(playerDetails);
}

void UPlayerDetailsWriter::WriteFromPlayerDetailsToJson(UPARAM(ref)FPlayerDetails& playerDetails)
{
	FJsonObjectWrapper jObjectWrapper;
	UJsonWriterBase::InitJsonObject(jObjectWrapper);

	// Set general details
	jObjectWrapper.JsonObject->SetStringField(FString{ "Name" }, playerDetails.mName);
	jObjectWrapper.JsonObject->SetNumberField(FString{ "Points" }, playerDetails.mPointsCollected);
	jObjectWrapper.JsonObject->SetBoolField(FString{ "FenderEnabled" }, playerDetails.bFender);

	// Set unlocked skins
	TSharedPtr<FJsonObject> SkinsUnlocked = MakeShareable(new FJsonObject());

	for (int i = 0; i < playerDetails.mSkinsUnlocked.Num(); i++)
	{
		SkinsUnlocked->SetBoolField(FString::FromInt(i), playerDetails.mSkinsUnlocked[i]);
	}

	jObjectWrapper.JsonObject->SetObjectField("SkinsUnlocked", SkinsUnlocked);

	// Set current player vehicle colours
	TArray<TSharedPtr<FJsonObject>> colours;

	// Init 4 colours (rails, bumper, footpad, fender)
	colours.Init(MakeShareable(new FJsonObject()), playerDetails.mBoardColours.Num());
	
	for (int i = 0; i < colours.Num(); i++)
	{
		colours[i]->SetNumberField(FString{ "R" }, playerDetails.mBoardColours[i].X);
		colours[i]->SetNumberField(FString{ "G" }, playerDetails.mBoardColours[i].Y);
		colours[i]->SetNumberField(FString{ "B" }, playerDetails.mBoardColours[i].Z);
	}

	jObjectWrapper.JsonObject->SetObjectField(FString{ "Rails" }, colours[1]);
	jObjectWrapper.JsonObject->SetObjectField(FString{ "Bumpers" }, colours[2]);
	jObjectWrapper.JsonObject->SetObjectField(FString{ "Footpads" }, colours[3]);
	jObjectWrapper.JsonObject->SetObjectField(FString{ "Fender" }, colours[4]);

	UJsonWriterBase::WriteJsonToFile(jObjectWrapper, FString{ "PlayerProfile" });
}

FPlayerDetails UPlayerDetailsWriter::LoadFromJsonToPlayerDetails()
{
	FJsonObjectWrapper jObject;
	if (!UJsonWriterBase::LoadJsonFileToWrapper(jObject, FString{ "PlayerProfile" }))
		return FPlayerDetails();

	FPlayerDetails playerDetails;

	// Get general details
	playerDetails.mName = jObject.JsonObject->GetStringField(FString{ "Name" });
	playerDetails.mPointsCollected = jObject.JsonObject->GetNumberField(FString{ "Points" });
	playerDetails.bFender = jObject.JsonObject->GetBoolField(FString{ "FenderEnabled" });

	// Get unlocked skins
	for (int i = 0; i < playerDetails.mSkinsUnlocked.Num(); i++)
	{
		playerDetails.mSkinsUnlocked[i] = jObject.JsonObject->GetObjectField(FString{ "UnlockedSkins" })->GetBoolField(FString::FromInt(i));
	}

	// Get current player vehicle colours
	TArray<FString> colourLabels{ "Rails", "Bumpers", "Footpads", "Fender" };
	for (int i = 0; i < colourLabels.Num(); i++)
	{
		FVector colour;
		colour.X = jObject.JsonObject->GetObjectField(colourLabels[i])->GetNumberField(FString{ "R" });
		colour.Y = jObject.JsonObject->GetObjectField(colourLabels[i])->GetNumberField(FString{ "G" });
		colour.Z = jObject.JsonObject->GetObjectField(colourLabels[i])->GetNumberField(FString{ "B" });

		playerDetails.mBoardColours.Add(colour);
	}

	return playerDetails;
}
