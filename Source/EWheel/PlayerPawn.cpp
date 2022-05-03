// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root component. Seperate from the mesh to avoid problems when rotating/tilting the board
	PlayerRoot = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerSphereCollider"));
	PlayerRoot->SetCollisionProfileName("Pawn");
	PlayerRoot->SetSphereRadius(15);
	PlayerRoot->SetSimulatePhysics(true);
	PlayerRoot->SetEnableGravity(true);
	PlayerRoot->OnComponentHit.AddDynamic(this, &APlayerPawn::OnMeshHit);
	//PlayerRoot->SetNotifyRigidBodyCollision(true); // (Simulation generates hit events enabled)
	PlayerRoot->SetLinearDamping(1.f);
	PlayerRoot->SetAngularDamping(100.f);
	PlayerRoot->SetConstraintMode(EDOFMode::Type::SixDOF);
	PlayerRoot->GetBodyInstance()->bLockYRotation = true;
	PlayerRoot->GetBodyInstance()->bLockXRotation = true;

	// Add the wheel mesh to the player mesh wheel socket
	static ConstructorHelpers::FObjectFinder<UStaticMesh>BoardMeshAsset(TEXT("StaticMesh'/Game/Meshes/EWheel/EWheel_CO_Board.EWheel_CO_Board'"));
	if (BoardMeshAsset.Succeeded())
	{
		BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMeshComponent"));
		BoardMesh->SetStaticMesh(BoardMeshAsset.Object);
		BoardMesh->SetSimulatePhysics(false);
		BoardMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		BoardMesh->SetupAttachment(PlayerRoot);

		static ConstructorHelpers::FObjectFinder<UStaticMesh>WheelMeshAsset(TEXT("StaticMesh'/Game/Meshes/EWheel/EWheel_CO_Wheel.EWheel_CO_Wheel'"));
		if (WheelMeshAsset.Succeeded())
		{
			WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelMeshComponent"));
			WheelMesh->SetStaticMesh(WheelMeshAsset.Object);
			WheelMesh->SetSimulatePhysics(false);
			WheelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			WheelMesh->SetupAttachment(BoardMesh);
		}
		static ConstructorHelpers::FObjectFinder<UStaticMesh>FenderMeshAsset(TEXT("StaticMesh'/Game/Meshes/EWheel/EWheel_Fender.EWheel_Fender'"));
		if (FenderMeshAsset.Succeeded())
		{
			FenderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FenderMeshComponent"));
			FenderMesh->SetStaticMesh(FenderMeshAsset.Object);
			FenderMesh->SetSimulatePhysics(false);
			FenderMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FenderMesh->SetupAttachment(BoardMesh);
		}
	}

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 60.f);
	SpringArm->SetRelativeRotation(FRotator(-17.f, 0.f, 0.f));
	SpringArm->SetupAttachment(PlayerRoot);
	SpringArm->TargetArmLength = 100.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 2.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 7.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 70.f;

	// Auto possess this pawn
	//AutoPossessPlayer = EAutoReceiveInput::Player0;
	//AutoReceiveInput = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update our ground contact. Enable hit events if the board is in the air to avoid constant hit registers from the ground 
	// (we need to check the ground tiles because of the mesh in the holes)
	if (!ValidateGroundContact() && !PlayerRoot->GetBodyInstance()->bNotifyRigidBodyCollision)
		PlayerRoot->SetNotifyRigidBodyCollision(true);
	else if (bWheelContact && PlayerRoot->GetBodyInstance()->bNotifyRigidBodyCollision)
	{
		PlayerRoot->SetNotifyRigidBodyCollision(false);
		bCanJump = true;
	}
	else if (GetActorLocation().Z < ZKillzone)
		KillPlayer();
			
	//Move the actor based on input
	MoveBoard(DeltaTime);
	//Rotate the actor based on input
	SetActorRotation(GetActorRotation() + FRotator{ 0, movementInput.X, 0 } *turnSpeed * DeltaTime);
	//Tilt the board in the direction of movement
	BoardTilt(DeltaTime);

	// Clamp Roll
	if (PlayerRoot->GetRelativeRotation().Roll > 45)
		PlayerRoot->SetRelativeRotation(FRotator{ PlayerRoot->GetRelativeRotation().Pitch, PlayerRoot->GetRelativeRotation().Yaw, 45.f });
	else if(PlayerRoot->GetRelativeRotation().Roll < -45)
		PlayerRoot->SetRelativeRotation(FRotator{ PlayerRoot->GetRelativeRotation().Pitch, PlayerRoot->GetRelativeRotation().Yaw, -45.f });

	// Log to HUD
	FVector2D playerLoc = { GetActorLocation().X, GetActorLocation().Y };
	distanceTravelled += (playerLoc - lastPlayerLocation).Size() * 0.01f;
	lastPlayerLocation = playerLoc;
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerPawn::MoveRight);
	PlayerInputComponent->BindAction("Escape", IE_Pressed, this, &APlayerPawn::Escape);
	PlayerInputComponent->BindAction("QuickRestart", IE_Pressed, this, &APlayerPawn::QuickRestart);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::Jump);

	// TEMP
	PlayerInputComponent->BindAction("TempSpeedIncrease", IE_Pressed, this, &APlayerPawn::LShiftDown);
	PlayerInputComponent->BindAction("TempSpeedIncrease", IE_Released, this, &APlayerPawn::LShiftUp);
}

void APlayerPawn::MoveBoard(float DeltaTime)
{
	FVector forwardDirection = GetActorForwardVector();
	forwardDirection.Z = 0;

	SetActorLocation(GetActorLocation() + forwardDirection * GetClaculatedSpeed(DeltaTime));

	// Roll wheel. Using Quats here to avoid gimbal locking
	GetWheelMesh()->AddLocalTransform(FTransform(FRotator{-GetClaculatedSpeed(DeltaTime), 0.f, 0.f }.Quaternion()));
}

float APlayerPawn::GetClaculatedSpeed(float DeltaTime)
{
	currentAcceleration = maxSpeed / accelerationRate * DeltaTime;

	if (movementInput.Y == 0)
		currentSpeed += currentAcceleration * -friction;
	else
		currentSpeed += currentAcceleration * movementInput.Y;

	currentSpeed = FMath::Clamp(currentSpeed, 0.f, maxSpeed);

	return currentSpeed * DeltaTime;
}

void APlayerPawn::BoardTilt(float DeltaTime)
{
	// Tilt the board forwards/back based on forward/back input
	currentBoardTilt.Pitch += movementInput.Y * boardTiltSpeed * DeltaTime;
	// Gradually reset the rotation of the board if there is no user input
	currentBoardTilt.Pitch += 0 - currentBoardTilt.Pitch * ((1 - FMath::Pow(movementInput.Y, 2.f)) / 1) * boardTiltResetSpeed * DeltaTime;
	// Clamp the rotation
	currentBoardTilt.Pitch = FMath::Clamp(currentBoardTilt.Pitch, -maxBoardTiltPitch, maxBoardTiltPitch);

	// Tilt the board left/right based on left/right input
	currentBoardTilt.Roll += movementInput.X * boardTiltSpeed * DeltaTime;
	// Gradually reset the rotation of the board if there is no user input
	currentBoardTilt.Roll += 0 - currentBoardTilt.Roll * ((1 - FMath::Pow(movementInput.X, 2.f)) / 1) * boardTiltResetSpeed * DeltaTime;
	// Clamp the rotation
	currentBoardTilt.Roll = FMath::Clamp(currentBoardTilt.Roll, -maxBoardTiltRoll, maxBoardTiltRoll);

	FRotator newRotation = GetBoardMesh()->GetRelativeRotation();
	newRotation.Pitch = -currentBoardTilt.Pitch;
	newRotation.Roll = currentBoardTilt.Roll;

	GetBoardMesh()->SetRelativeRotation(newRotation);
}

bool APlayerPawn::ValidateGroundContact()
{	
	FVector RaycastStartPos = GetActorLocation() - GetActorUpVector() * groundContactRayOffset;
	FVector RaycastEndPos = GetActorLocation() - GetActorUpVector() * (groundContactRayOffset + groundContactRayLength);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FHitResult ray;

	//DrawDebugLine(GetWorld(), RaycastStartPos, RaycastEndPos, FColor::Green, false);
	bWheelContact = GetWorld()->LineTraceSingleByChannel(ray, RaycastStartPos, RaycastEndPos, ECC_Visibility, CollisionParams);

	//FVector newRot = ray.ImpactNormal;
	//GetBoardMesh()->AddRelativeRotation(newRot * 0.001f);

	return bWheelContact;
}

void APlayerPawn::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	float collisionAngle = FVector::DotProduct(GetActorForwardVector(), Hit.ImpactNormal);
	if (collisionAngle < -0.785398 || collisionAngle > 0.785398) // 45 degrees
		KillPlayer();
}

void APlayerPawn::MoveForward(float input)
{
	movementInput.Y = input;
}

void APlayerPawn::MoveRight(float input)
{
	movementInput.X = input;
}

void APlayerPawn::Escape()
{
	UE_LOG(LogTemp, Warning, TEXT("Broadcasted EscPressed"));
	EscPressed.Broadcast();
}

void APlayerPawn::QuickRestart()
{
	RestartPressed.Broadcast();
}

void APlayerPawn::Jump()
{
	if (bCanJump)
	{
		PlayerRoot->AddImpulse(GetActorUpVector() * jumpForce);
		bCanJump = false;
	}
}

void APlayerPawn::LShiftDown()
{
	maxSpeed = 833.33f; // 30km/h
}

void APlayerPawn::LShiftUp()
{
	maxSpeed = 600.f; // 21.6km/h
}

void APlayerPawn::KillPlayer()
{
	GetBoardMesh()->SetCollisionProfileName("VehicleComponent");
	GetBoardMesh()->SetSimulatePhysics(true);
	GetBoardMesh()->AddImpulse(-GetActorForwardVector() * 250.f + FVector{ 0, 0, 0.5f});
	GetWheelMesh()->SetCollisionProfileName("VehicleComponent");
	GetWheelMesh()->SetSimulatePhysics(true);
	GetWheelMesh()->AddImpulse(-GetActorForwardVector() * 50.f + FVector{ 0, 0, 0.5f });
	SpringArm->TargetArmLength = 400.0f;
	
	bIsDead = true;
	SetActorTickEnabled(false);
	PlayerDeath.Broadcast();
	PlayerDeath.Clear();
}