// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/JsonWriters/PlayerDetailsWriter.h"
#include "EWheel/JsonWriters/JsonWriterBase.h"

void UPlayerDetailsWriter::SetPlayerName(const FString& playerName)
{
	FPlayerDetails playerDetails;
	LoadFromJsonToPlayerDetails(playerDetails);

	playerDetails.mName = playerName;
	WriteFromPlayerDetailsToJson(playerDetails);
}

void UPlayerDetailsWriter::SetPlayerPoints(int points)
{
	FPlayerDetails playerDetails;
	LoadFromJsonToPlayerDetails(playerDetails);

	playerDetails.mPointsCollected = points;
	WriteFromPlayerDetailsToJson(playerDetails);
}

void UPlayerDetailsWriter::SetPlayerFenderEnabled(bool bFender)
{
	FPlayerDetails playerDetails;
	LoadFromJsonToPlayerDetails(playerDetails);

	playerDetails.bFender = bFender;
	WriteFromPlayerDetailsToJson(playerDetails);
}

void UPlayerDetailsWriter::SetPlayerSkinUnlocked(int index, bool bUnlocked)
{
	FPlayerDetails playerDetails;
	LoadFromJsonToPlayerDetails(playerDetails);

	if (index < playerDetails.mSkinsUnlocked.Num())
	{
		playerDetails.mSkinsUnlocked[index] = bUnlocked;
		WriteFromPlayerDetailsToJson(playerDetails);
	}
}

void UPlayerDetailsWriter::SetPlayerColours(UPARAM(ref)TArray<FVector>& colours)
{
	FPlayerDetails playerDetails;
	LoadFromJsonToPlayerDetails(playerDetails);

	if (playerDetails.mBoardColours.Num() != colours.Num())
		return;

	for (int i = 0; i < 4; i++)
	{
		playerDetails.mBoardColours[i].X = colours[i].X;
		playerDetails.mBoardColours[i].Y = colours[i].Y;
		playerDetails.mBoardColours[i].Z = colours[i].Z;
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

	jObjectWrapper.JsonObject->SetObjectField("UnlockedSkins", SkinsUnlocked);

	// Set current player vehicle colours
	TSharedPtr<FJsonObject> rails = MakeShareable(new FJsonObject());
	TSharedPtr<FJsonObject> bumpers = MakeShareable(new FJsonObject());
	TSharedPtr<FJsonObject> footpads = MakeShareable(new FJsonObject());
	TSharedPtr<FJsonObject> fender = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonObject>> colours{ rails, bumpers, footpads, fender };
	
	for (int i = 0; i < colours.Num(); i++)
	{
		colours[i]->SetNumberField(FString{ "R" }, playerDetails.mBoardColours[i].X);
		colours[i]->SetNumberField(FString{ "G" }, playerDetails.mBoardColours[i].Y);
		colours[i]->SetNumberField(FString{ "B" }, playerDetails.mBoardColours[i].Z);
	}

	jObjectWrapper.JsonObject->SetObjectField(FString{ "Rails" }, colours[0]);
	jObjectWrapper.JsonObject->SetObjectField(FString{ "Bumpers" }, colours[1]);
	jObjectWrapper.JsonObject->SetObjectField(FString{ "Footpads" }, colours[2]);
	jObjectWrapper.JsonObject->SetObjectField(FString{ "Fender" }, colours[3]);

	UJsonWriterBase::WriteJsonToFile(jObjectWrapper, FString{ "PlayerProfile" });
}

FPlayerDetails& UPlayerDetailsWriter::LoadFromJsonToPlayerDetails(UPARAM(ref)FPlayerDetails& playerDetails)
{
	FJsonObjectWrapper jObject;
	if (!UJsonWriterBase::LoadJsonFileToWrapper(jObject, FString{ "PlayerProfile" }))
		return playerDetails;

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
		colour.X = jObject.JsonObject->GetObjectField(*colourLabels[i])->GetNumberField(FString{ "R" });
		colour.Y = jObject.JsonObject->GetObjectField(*colourLabels[i])->GetNumberField(FString{ "G" });
		colour.Z = jObject.JsonObject->GetObjectField(*colourLabels[i])->GetNumberField(FString{ "B" });

		playerDetails.mBoardColours[i] = colour;
	}

	return playerDetails;
}
