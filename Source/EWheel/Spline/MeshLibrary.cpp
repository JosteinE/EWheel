// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

MeshLibrary::MeshLibrary()
{
	TArray<FString> meshPaths;
	// Default ground
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/DefaultGround_150x150_Sub.DefaultGround_150x150_Sub'");
	// Pits
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_150x150.Ground_Pit_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_4W_150x150.Ground_Pit_4W_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_EndP_SN_150x150.Ground_Pit_EndP_SN_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_L_SNW_150x150.Ground_Pit_L_SNW_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_T_SN_150x150.Ground_Pit_T_SN_150x150'");
	// Ramps
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_Single_150x150.Ground_Ramp_Single_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_NW_150x150.Ground_Ramp_NW_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_N_150x150.Ground_Ramp_N_150x150'");
	meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_NE_150x150.Ground_Ramp_NE_150x150'");

	for (int i = 0; i < meshPaths.Num(); i++)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(*meshPaths[i]);
		if (MeshAsset.Succeeded())
			mLibrary.Emplace(MeshAsset.Object);
	}
}

MeshLibrary::~MeshLibrary()
{
}

UStaticMesh* MeshLibrary::GetMesh(int meshType)
{
	return mLibrary[meshType];
}
