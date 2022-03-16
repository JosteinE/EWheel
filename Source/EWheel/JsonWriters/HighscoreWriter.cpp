// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/JsonWriters/HighscoreWriter.h"
#include "EWheel/JsonWriters/JsonWriterBase.h"

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
	UJsonWriterBase::InitJsonObject(highscoreSlotJsonWrapper);
	FString fileName = inMode.ToUpper() + "Highscores";

	if (!UJsonWriterBase::LoadJsonFileToWrapper(highscoreJsonWrapper, fileName))
	{
		UJsonWriterBase::InitJsonObject(highscoreJsonWrapper);
		ImportFromHighscoreData(highscoreSlotJsonWrapper, inPlayer);
		highscoreJsonWrapper.JsonObject->SetObjectField(FString("1"), highscoreSlotJsonWrapper.JsonObject);
	}
	else
	{
		bool bMoveDown = false;
		for(int i = 1; i < 16; i++)
		{
			const TSharedPtr<FJsonObject>* jObject;

			// Check if the file object exists
			if (!highscoreJsonWrapper.JsonObject->TryGetObjectField(FString::FromInt(i), jObject))
			{
				if (!bMoveDown)
				{
					ImportFromHighscoreData(highscoreSlotJsonWrapper, inPlayer);
					highscoreJsonWrapper.JsonObject->SetObjectField(FString::FromInt(i), highscoreSlotJsonWrapper.JsonObject);
				}
				break;
			}

			// Compare to rank = i
			if (!bMoveDown && (*jObject)->GetNumberField("Score") < inPlayer.mScore)
			{
				bMoveDown = true;
				ImportFromHighscoreData(highscoreSlotJsonWrapper, inPlayer);
				highscoreJsonWrapper.JsonObject->SetObjectField(FString::FromInt(i), highscoreSlotJsonWrapper.JsonObject);
			}

			// Add object to the highscore wrapper
			if (i < 14 || (i == 14 && bMoveDown) || (i == 15 && !bMoveDown))
				highscoreJsonWrapper.JsonObject->SetObjectField(FString::FromInt(i + bMoveDown), *jObject);
		}
	}

	UJsonWriterBase::WriteJsonToFile(highscoreJsonWrapper, fileName);
}

void HighscoreWriter::ImportFromHighscoreData(UPARAM(ref)FJsonObjectWrapper& jObjectWrapper, HighscoreSlot& inPlayer)
{
	jObjectWrapper.JsonObject->SetStringField("Name", inPlayer.mName);
	jObjectWrapper.JsonObject->SetNumberField("Score", inPlayer.mScore);
	jObjectWrapper.JsonObject->SetStringField("Time", inPlayer.mTime);
	jObjectWrapper.JsonObject->SetNumberField("Distance", inPlayer.mDistance);
}
