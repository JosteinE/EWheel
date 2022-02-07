// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum MeshTypes
{
	DEFAULT,
	PIT,
	PIT4W,
	PIT_END_SN,
	PIT_EX,
	PIT_L,
	PIT_T,
	RAMP,
	RAMP_L,
	RAMP_M,
	RAMP_R
};

class UStaticMesh;

/**
 * 
 */
class EWHEEL_API MeshLibrary
{
public:
	MeshLibrary();
	~MeshLibrary();

	UStaticMesh* GetMesh(int meshType);
private:
	TArray<UStaticMesh*> mLibrary;
};
