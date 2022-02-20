// Fill out your copyright notice in the Description page of Project Settings.


#include "EWheel/Spline/MeshLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

MeshLibrary::MeshLibrary()
{
	TArray<FString> meshPaths;
	TArray<FString> edgeMeshPaths;

	// Default ground
	if (bUseHighResModels)
	{
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/DefaultGround_150x150_Sub_Smooth.DefaultGround_150x150_Sub_Smooth'");
		// Pits
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_150x150_Smooth.Ground_Pit_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_4W_150x150_Smooth.Ground_Pit_4W_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_EndP_SN_150x150_Smooth.Ground_Pit_EndP_SN_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_Ex_SN_150x150_Smooth.Ground_Pit_Ex_SN_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_L_SNW_150x150_Smooth.Ground_Pit_L_SNW_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Pit_T_SN_150x150_Smooth.Ground_Pit_T_SN_150x150_Smooth'");
		// Ramps
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_Single_150x150_Smooth.Ground_Ramp_Single_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_NW_150x150_Smooth.Ground_Ramp_NW_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_N_150x150_Smooth.Ground_Ramp_N_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/Ground_Ramp_NE_150x150_Smooth.Ground_Ramp_NE_150x150_Smooth'");
		// Hole
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/Hole/PathEdge_Hole_Single_150x150_Smooth.PathEdge_Hole_Single_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/Hole/PathEdge_Hole_4W_150x150_Smooth.PathEdge_Hole_4W_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/Hole/PathEdge_Hole_EndP_SN_150x150_Smooth.PathEdge_Hole_EndP_SN_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/Hole/PathEdge_Hole_Ex_150x150_Smooth.PathEdge_Hole_Ex_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/Hole/PathEdge_Hole_L_150x150_Smooth.PathEdge_Hole_L_150x150_Smooth'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/Hole/PathEdge_Hole_T_150x150_Smooth.PathEdge_Hole_T_150x150_Smooth'");
	
		// Edge Meshes
		// Dive
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Dive_L_150x150_Smooth.PathEdge_Dive_L_150x150_Smooth'");
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Dive_R_150x150_Smooth.PathEdge_Dive_R_150x150_Smooth'");
		// Hill
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Hill_L_150x150_Smooth.PathEdge_Hill_L_150x150_Smooth'");
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Hill_R_150x150_Smooth.PathEdge_Hill_R_150x150_Smooth'");
		// Slope
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Slope_L_150x150_Smooth.PathEdge_Slope_L_150x150_Smooth'");
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Slope_R_150x150_Smooth.PathEdge_Slope_R_150x150_Smooth'");
		// Hole
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Hole_Edge_L_150x150_Smooth.PathEdge_Hole_Edge_L_150x150_Smooth'");
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/GroundTiles/PathEdge/PathEdge_Hole_Edge_R_150x150_Smooth.PathEdge_Hole_Edge_R_150x150_Smooth'");
	}
	else
	{
		// OLD
		// Default ground
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/DefaultGround_150x150_Sub.DefaultGround_150x150_Sub'");
		// Pits
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Pit_150x150.Ground_Pit_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Pit_4W_150x150.Ground_Pit_4W_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Pit_EndP_SN_150x150.Ground_Pit_EndP_SN_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Pit_Ex_SN_150x150.Ground_Pit_Ex_SN_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Pit_L_150x150.Ground_Pit_L_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Pit_T_SN_150x150.Ground_Pit_T_SN_150x150'");
		// Ramps
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Ramp_Single_150x150.Ground_Ramp_Single_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Ramp_NW_150x150.Ground_Ramp_NW_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Ramp_N_150x150.Ground_Ramp_N_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/Ground_Ramp_NE_150x150.Ground_Ramp_NE_150x150'");
		// Hole
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/Hole/PathEdge_Hole_Single_150x150.PathEdge_Hole_Single_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/Hole/PathEdge_Hole_4W_150x150.PathEdge_Hole_4W_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/Hole/PathEdge_Hole_EndP_SN_150x150.PathEdge_Hole_EndP_SN_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/Hole/PathEdge_Hole_Ex_150x150.PathEdge_Hole_Ex_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/Hole/PathEdge_Hole_L_150x150.PathEdge_Hole_L_150x150'");
		meshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/Hole/PathEdge_Hole_T_150x150.PathEdge_Hole_T_150x150'");

		// Edge Meshes
		// Dive
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Dive_L_150x150.PathEdge_Dive_L_150x150'");
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Dive_R_150x150.PathEdge_Dive_R_150x150'");
		// Hill
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Hill_L_150x150.PathEdge_Hill_L_150x150'");
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Hill_R_150x150.PathEdge_Hill_R_150x150'");
		// Slope
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Slope_L_150x150.PathEdge_Slope_L_150x150'");
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Slope_R_150x150.PathEdge_Slope_R_150x150'");
		// Hole
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Hole_Edge_L_150x150.PathEdge_Hole_Edge_L_150x150'");
		edgeMeshPaths.Emplace("StaticMesh'/Game/Meshes/OLDGroundTiles/PathEdge/PathEdge_Hole_Edge_R_150x150.PathEdge_Hole_Edge_R_150x150'");
	}

	for (int i = 0; i < meshPaths.Num(); i++)
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(*meshPaths[i]);
		if (MeshAsset.Succeeded())
			mLibrary.Emplace(Cast<UStaticMesh>(MeshAsset.Object));
	}
	for (int i = 0; i < edgeMeshPaths.Num(); i++)
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(*edgeMeshPaths[i]);
		if (MeshAsset.Succeeded())
			mEdgeMeshLibrary.Emplace(Cast<UStaticMesh>(MeshAsset.Object));
	}
}

MeshLibrary::~MeshLibrary()
{
}

UStaticMesh* MeshLibrary::GetMesh(int meshType)
{
	return mLibrary[meshType];
}

UStaticMesh* MeshLibrary::GetEdgeMesh(int edgeMeshType)
{
	return mEdgeMeshLibrary[edgeMeshType];
}
