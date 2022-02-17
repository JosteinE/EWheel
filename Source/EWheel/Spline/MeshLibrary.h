// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UStaticMesh;

/** Contains all of the different mesh types, specifically listed in the order of the MeshType enum in MeshCategoriesAndTypes
 * 
 */
class EWHEEL_API MeshLibrary
{
public:
	MeshLibrary();
	~MeshLibrary();

	UStaticMesh* GetMesh(int meshType);
	UStaticMesh* GetEdgeMesh(int edgeMeshType);
private:
	TArray<UStaticMesh*> mLibrary;
	TArray<UStaticMesh*> mEdgeMeshLibrary;
};
