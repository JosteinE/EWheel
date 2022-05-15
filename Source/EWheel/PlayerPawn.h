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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuickRestartPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);

UCLASS()
class EWHEEL_API APlayerPawn : public APawn
{
	GENERATED_BODY()

	/** ############################################## */
	UPROPERTY(Category = Root, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* PlayerRoot;

	/** Static Mesh component that will represent the player's board */
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BoardMesh;

	/** Static Mesh component that will represent our player's wheel */
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WheelMesh;

	/** Static Mesh component that will represent our player's fender  */
	UPROPERTY(Category = Mesh, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FenderMesh;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Forward movement speed of the vehicle */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float maxSpeed = 600.f; // 600 = 21.6km/h,  833.33f = 30km/h

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

	/** Jump force of the vehicle */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float jumpForce = 2000.f;

	/** How fast the vehicle naturally slows down without input (0-1)  */
	UPROPERTY(Category = VehicleSpecs, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float friction = 0.5f;

	/** True if the wheel is in the right orientation and contact with the ground was successfully validated  */
	UPROPERTY(Category = VehicleSpecs, BlueprintReadOnly, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bWheelContact = true;

public:
	/** Number of points collected */
	UPROPERTY(Category = HUD, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int pointsCollected = 0;

	/** Distance Travelled along the path */
	UPROPERTY(Category = HUD, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float distanceTravelled = 0.f;

	/** The player is killed if he falls below this value */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ZKillzone = 0.f;

	UPROPERTY(Category = Player, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

	UPROPERTY(Category = Player, VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString PlayerName = "Player";

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

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowGameSummary(bool bWithSave);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayPointSound(bool bFromObstacle);

	// Current directional input
	FVector movementInput;

private:
	FRotator currentBoardTilt;
	float maxBoardTiltPitch = 15.f;
	float maxBoardTiltRoll = 35;
	float boardTiltSpeed = 100.f;
	float boardTiltResetSpeed = 10.f;
	float groundContactRayOffset = 10.f;
	float groundContactRayLength = 15;//2.f;
	//float groundContactRaySideOffset = 5.f;
	bool bCanJump = true;

	// To track distance traveled
	FVector2D lastPlayerLocation;

	// Move the board based on user input
	void MoveBoard(float DeltaTime);

	// Tilt the board based on direction of movement
	void BoardTilt(float DeltaTime);

	// Raycasts that determine if the board has sufficient contact with the ground 
	bool ValidateGroundContact();

	// Returns the appropriate speed value given input duration
	float GetClaculatedSpeed(float DeltaTime);


	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// User inputs
	/** Handle pressing forwards */
	void MoveForward(float input);
	/** Handle pressing right */
	void MoveRight(float input);
	/** Handle pressing Escape */
	void Escape();
	/** Handle pressing R */
	void QuickRestart();
	/** Handle pressing Space */
	void Jump();
	/** TEMP Handle pressing Shift TEMP */
	void LShiftDown();
	/** TEMP Handle releasing Shift TEMP */
	void LShiftUp();
public:
	// Delegate Signatures
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnEscapePressed EscPressed;

	// Delegate Signatures
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnQuickRestartPressed RestartPressed;

	// Delegate Signatures
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnPlayerDeath PlayerDeath;

	// Enables physics simulatioon and broadcasts death to the game mode
	void KillPlayer();

	/** Returns the player's wheel mesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetWheelMesh() const { return WheelMesh; }
	/** Returns the player's board mesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetBoardMesh() const { return BoardMesh; }
	/** Returns the player's fender mesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetFenderMesh() const { return FenderMesh; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns CurrentSpeed **/
	FORCEINLINE float GetCurrentSpeed() const { return currentSpeed; }
	/** Returns CurrentSpeed **/
	FORCEINLINE float GetMaxSpeed() const { return maxSpeed; }

	/** Adds the value to the player score **/
	FORCEINLINE void AddToScore(int value, bool bFromObstacle) { pointsCollected += value; PlayPointSound(bFromObstacle); }
};
