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
	UPROPERTY(Category = Root, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* PlayerRoot;

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

	/** Current speed of the vehicle */
	UPROPERTY(Category = VehicleSpecs, VisibleAnywhere)
	float currentSpeed = 0.f;

	/** Acceleration Rate of the vehicle in seconds (MaxSpeed/Seconds) */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere)
	float accelerationRate = 2.f;

	/** Current acceleration of the vehicle */
	UPROPERTY(Category = VehicleSpecs, VisibleAnywhere)
	float currentAcceleration = 0.f;

	/** Turn speed of the vehicle */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere)
	float turnSpeed = 100.f;

	/** How fast the vehicle naturally slows down without input (0-1)  */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere)
	float friction = 0.5f;

	/** True if the wheel is in the right orientation and contact with the ground was successfully validated  */
	UPROPERTY(Category = VehicleSpecs, VisibleAnywhere)
	bool bWheelContact = true;

	/** True if the board is colliding with the ground  */
	UPROPERTY(Category = VehicleSpecs, VisibleAnywhere)
	bool bIsCollidingWithGround = false;

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

private:
	FVector movementInput;
	FRotator currentBoardTilt;
	float maxBoardTiltPitch = 15.f;
	float maxBoardTiltRoll = 35;
	float boardTiltSpeed = 100.f;
	float boardTiltResetSpeed = 10.f;
	float groundContactRayOffset = 50.f;
	float groundContactRayLength = 25.f;
	float groundContactRaySideOffset = 30.f;

	void MoveBoard(float DeltaTime);
	void BoardTilt(float DeltaTime);
	bool ValidateGroundContact(); // 3x Raycasts that determine if the board has sufficient contact with the ground to keep moving  
	float GetClaculatedSpeed(float DeltaTime);


	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// User inputs
	/** Handle pressing forwards */
	void MoveForward(float input);
	/** Handle pressing right */
	void MoveRight(float input);

public:


	/** Returns the player mesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return PlayerMesh; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
};
