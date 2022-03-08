// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/JsonWriters/HighscoreWriter.h"

HighscoreWriter::HighscoreWriter()
{
}

HighscoreWriter::~HighscoreWriter()
{
}

void HighscoreWriter::AddToHighscore(HighscoreSlot& inPlayer, FString& inMode)
{
	FJsonObjectWrapper highscoreJsonWrapper;
	FJsonObjectWrapper highscoreSlotJsonWrapper;
	FString fileName = inMode + "Highscore";

	if (!LoadJsonFileToWrapper(highscoreJsonWrapper, fileName))
	{
		InitJsonObject(highscoreJsonWrapper);
		InitJsonObject(highscoreSlotJsonWrapper);
		ImportFromHighscoreData(highscoreSlotJsonWrapper, inPlayer);
		highscoreJsonWrapper.JsonObject->SetObjectField(FString{ "1" }, highscoreSlotJsonWrapper.JsonObject);
	}
	else
	{
		bool bMoveDown = false;
		for(int i = 1; i < 16; i++)
		{
			TSharedPtr<FJsonObject> jObject = (highscoreJsonWrapper.JsonObject->GetObjectField(FString::FromInt(i)));
			if (!jObject)
				break;

			if (!bMoveDown && jObject->GetNumberField("Score") < inPlayer.mScore)
			{
				bMoveDown = true;
				ImportFromHighscoreData(highscoreSlotJsonWrapper, inPlayer);
				highscoreJsonWrapper.JsonObject->SetObjectField(FString::FromInt(i), highscoreSlotJsonWrapper.JsonObject);
			}

			if(i < 14 || (i == 14 && bMoveDown) || (i == 15 && !bMoveDown))
				highscoreJsonWrapper.JsonObject->SetObjectField(FString::FromInt(i + bMoveDown), jObject);
		}
	}

	WriteJsonToFile(highscoreJsonWrapper, fileName);
}

void HighscoreWriter::ImportFromHighscoreData(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, HighscoreSlot& inPlayer)
{
	jObjectWrapper.JsonObject->SetStringField("Name", inPlayer.mName);
	jObjectWrapper.JsonObject->SetNumberField("Score", inPlayer.mScore);
	jObjectWrapper.JsonObject->SetStringField("Time", inPlayer.mTime);
	jObjectWrapper.JsonObject->SetNumberField("Distance", inPlayer.mDistance);
}
