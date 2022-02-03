// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class UPhysicsConstraintComponent;
class USphereComponent;

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEscapePressed);

UCLASS()
class EWHEEL_API APlayerPawn : public APawn
{
	GENERATED_BODY()

	/** ############################################## */
	UPROPERTY(Category = Root, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* PlayerRoot;

	/** Static Mesh component that will represent the wheel */
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BoardMesh;

	/** Static Mesh component that will represent our Player */
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WheelMesh;

	/** Component that binds the board and wheel mesh while allowing both to have simulate physics enabled */
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* PhysicsConstraintComponent;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Forward movement speed of the vehicle */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float maxSpeed = 10.f;

	/** Current speed of the vehicle */
	UPROPERTY(Category = VehicleSpecs, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float currentSpeed = 0.f;

	/** Acceleration Rate of the vehicle in seconds (MaxSpeed/Seconds) */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float accelerationRate = 2.f;

	/** Current acceleration of the vehicle */
	UPROPERTY(Category = VehicleSpecs, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	float currentAcceleration = 0.f;

	/** Turn speed of the vehicle */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float turnSpeed = 100.f;

	/** How fast the vehicle naturally slows down without input (0-1)  */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float friction = 0.5f;

	/** True if the wheel is in the right orientation and contact with the ground was successfully validated  */
	UPROPERTY(Category = VehicleSpecs, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bWheelContact = true;

	/** Current speed of the vehicle */
	UPROPERTY(Category = Score, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int pointsCollected = 0;

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
	float groundContactRayOffset = 0.f;
	float groundContactRayLength = 5.f;
	float groundContactRaySideOffset = 5.f;

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
	/** Handle pressing right */
	void Escape();

public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnEscapePressed EscPressed;

	/** Returns the player mesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetWheelMesh() const { return WheelMesh; }
	/** Returns the player's wheel mesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetBoardMesh() const { return BoardMesh; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	/** Adds the value to the player score **/
	FORCEINLINE void AddToScore(int value) { pointsCollected += value; }
};
