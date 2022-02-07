// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class EWHEEL_API SplineTilePicker
{
public:
	SplineTilePicker();
	~SplineTilePicker();

private:
	// MeshType and Num 90 degree rotations
	TArray<TPair<int, int>> TileLog;
};
