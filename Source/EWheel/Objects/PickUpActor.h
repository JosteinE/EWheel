// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpActor.generated.h"

class UStaticMeshComponent;

UCLASS()
class EWHEEL_API APickUpActor : public AActor
{
	GENERATED_BODY()
	
	/** Mesh component belonging to the pickup object */
	UPROPERTY(Category = Mesh, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

public:	
	// Sets default values for this actor's properties
	APickUpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetStaticMesh(FString& inPath);
	void SetStaticMesh(UStaticMesh* inMesh);

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	/** Returns the object's mesh component **/
	FORCEINLINE UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
};
