// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EWheel/Objects/ObstacleActor.h"
#include "ObstacleCube.generated.h"

UCLASS()
class EWHEEL_API AObstacleCube : public AObstacleActor
{
	GENERATED_BODY()


	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MidMeshComponent;

	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* TopMeshComponent;
public:	
	// Sets default values for this actor's properties
	AObstacleCube();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void OnConstruction(const FTransform& Transform) override;

public:
	UFUNCTION()
		void SetHeight(float height);
};