// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;

UCLASS()
class EWHEEL_API APlayerPawn : public APawn
{
	GENERATED_BODY()

	/** Static Mesh component that will represent our Player */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlayerMesh;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Forward movement speed of the vehicle */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere)
	float maxSpeed = 10.f;

	/** Acceleration Rate of the vehicle in seconds (MaxSpeed/Seconds) */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere)
	float accelerationRate = 2.f;

	/** Current speed of the vehicle */
	UPROPERTY(Category = VehicleSpecs, VisibleAnywhere)
	float currentSpeed = 0.f;

	/** Turn speed of the vehicle */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere)
	float turnSpeed = 50.f;

	/** How fast the vehicle naturally slows down without input (0-1)  */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere)
	float friction = 0.5f;

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector movementInput;

	float GetClaculatedSpeed(float DeltaTime);

	// User inputs
	/** Handle pressing forwards */
	void MoveForward(float input);
	/** Handle pressing right */
	void MoveRight(float input);


	/** Returns the player mesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return PlayerMesh; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
};
