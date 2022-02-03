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

	// Player mesh
	//PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMeshComponent"));
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh>BoardMesh(TEXT("SkeletalMesh'/Game/Meshes/EWheel/EWheel.EWheel'"));
	//if (BoardMesh.Succeeded())
	//	GetMesh()->SetSkeletalMesh(BoardMesh.Object);
	//PlayerMesh->SetCollisionProfileName("Pawn");
	//PlayerMesh->OnComponentHit.AddDynamic(this, &APlayerPawn::OnMeshHit);
	//RootComponent = PlayerMesh;

	//static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	//GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Root component. Seperate from the mesh to avoid problems when rotating/tilting the board
	PlayerRoot = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerSphereCollider"));
	PlayerRoot->SetCollisionProfileName("Pawn");
	PlayerRoot->SetSphereRadius(15);
	PlayerRoot->SetSimulatePhysics(true);
	PlayerRoot->SetEnableGravity(true);
	PlayerRoot->OnComponentHit.AddDynamic(this, &APlayerPawn::OnMeshHit);
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
			//WheelMesh->AttachToComponent(BoardMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false), "WheelSocket");
			//PhysicsConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraintComponent"));
			//PhysicsConstraintComponent->AttachTo(PlayerRoot, PlayerRoot->GetFName(), EAttachLocation::KeepWorldPosition);
		}
	}

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 60.f);
	SpringArm->SetRelativeRotation(FRotator(-12.f, 0.f, 0.f));
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
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoReceiveInput = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	// Prevent seperation between the wheel and board using the PhysicsConstraintComponent
	//if (PhysicsConstraintComponent)
	//{
	//	FConstraintInstance Constraint;
	//	Constraint.ProfileInstance.bDisableCollision = true;
	//	Constraint.ProfileInstance.bParentDominates = true;
	//	Constraint.SetAngularSwing1Motion(EAngularConstraintMotion::ACM_Locked);
	//	Constraint.SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Locked);
	//	Constraint.SetAngularTwistMotion(EAngularConstraintMotion::ACM_Locked);
	//	Constraint.ProfileInstance.LinearLimit.bSoftConstraint = false;
	//	Constraint.ProfileInstance.TwistLimit.bSoftConstraint = false;
	//	PhysicsConstraintComponent->ConstraintInstance = Constraint;
	//	PhysicsConstraintComponent->SetConstrainedComponents(WheelMesh, NAME_None, BoardMesh, NAME_None);
	//}
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//// Simulate physics if the board is not colliding
	//if (!ValidateGroundContact() && !PlayerMesh->IsSimulatingPhysics())
	//	PlayerMesh->SetSimulatePhysics(true);
	//else if (bWheelContact && PlayerMesh->IsSimulatingPhysics() && bIsCollidingWithGround)
	//	PlayerMesh->SetSimulatePhysics(false);
	//bIsCollidingWithGround = false;

	if(!GetBoardMesh()->IsSimulatingPhysics())
	{
		//Move the actor based on input
		MoveBoard(DeltaTime);
		//Rotate the actor based on input
		SetActorRotation(GetActorRotation() + FRotator{ 0, movementInput.X, 0 } *turnSpeed * DeltaTime);
		//Tilt the board in the direction of movement
		BoardTilt(DeltaTime);

		if (PlayerRoot->GetRelativeRotation().Roll > 45)
			PlayerRoot->SetRelativeRotation(FRotator{ PlayerRoot->GetRelativeRotation().Pitch, PlayerRoot->GetRelativeRotation().Yaw, 45.f });
		else if(PlayerRoot->GetRelativeRotation().Roll < -45)
			PlayerRoot->SetRelativeRotation(FRotator{ PlayerRoot->GetRelativeRotation().Pitch, PlayerRoot->GetRelativeRotation().Yaw, -45.f });
	}
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
}

void APlayerPawn::MoveBoard(float DeltaTime)
{
	FVector forwardDirection = GetActorForwardVector();
	forwardDirection.Z = 0;
	SetActorLocation(GetActorLocation() + forwardDirection * GetClaculatedSpeed(DeltaTime));
	//GetMesh()->AddForce(forwardDirection * GetClaculatedSpeed(DeltaTime) * 1000.f);
	//GetMesh()->AddTorque(GetActorRightVector() * movementInput.Y * 10000.f);

	// To avoid gimbal locking
	GetWheelMesh()->AddLocalTransform(FTransform(FRotator{-GetClaculatedSpeed(DeltaTime), 0.f, 0.f }.Quaternion()));
	//GetWheelMesh()->AddRelativeRotation(FRotator{ -GetClaculatedSpeed(DeltaTime), 0.f, 0.f });
	//UE_LOG(LogTemp, Warning, TEXT("BoardRott: %f, %f, %f"), GetWheelMesh()->GetRelativeRotation().Pitch, GetWheelMesh()->GetRelativeRotation().Yaw, GetWheelMesh()->GetRelativeRotation().Roll);
}

float APlayerPawn::GetClaculatedSpeed(float DeltaTime)
{
	currentAcceleration = maxSpeed / accelerationRate * DeltaTime;

	if (movementInput.Y == 0)
		currentSpeed += currentAcceleration * -friction;
	else
		currentSpeed += currentAcceleration * movementInput.Y;

	currentSpeed = FMath::Clamp(currentSpeed, 0.f, maxSpeed);
	//currentSpeed = FMath::Clamp(currentSpeed + currentAcceleration, 0.f, maxSpeed);

	return currentSpeed;
}

void APlayerPawn::BoardTilt(float DeltaTime)
{
	//currentBoardTilt = FMath::Clamp(currentBoardTilt + movementInput.Y * boardTiltSpeed * DeltaTime, -maxBoardTiltRotation, maxBoardTiltRotation);
	//if (movementInput.Y == 0) <----BORING
	//	currentBoardTilt += 0 - currentBoardTilt * DeltaTime;

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

	//Move the pivot the board around the center of the wheel by first moving the mesh origin to the center of the wheel, rotating, then moving it back
	//Couldnt get this to work without the board visually teleporting in game
	//FVector centerOfWheel = GetActorLocation() + GetActorUpVector() * 15.f;
	//GetMesh()->SetWorldLocation(centerOfWheel - GetActorLocation());
	GetBoardMesh()->SetRelativeRotation(newRotation);
	
	//GetMesh()->SetWorldLocation(GetActorLocation() - centerOfWheel);
}

bool APlayerPawn::ValidateGroundContact()
{	
	FVector RaycastStartPos = GetActorLocation() - GetActorUpVector() * groundContactRayOffset;
	FVector RaycastEndPos = GetActorLocation() - GetActorUpVector() * (groundContactRayOffset + groundContactRayLength);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	FHitResult ray;

	DrawDebugLine(GetWorld(), RaycastStartPos, RaycastEndPos, FColor::Green, false);
	return GetWorld()->LineTraceSingleByChannel(ray, RaycastStartPos, RaycastEndPos, ECC_Visibility, CollisionParams);
	//FVector RaycastStartPos = GetActorLocation() - GetActorUpVector() * groundContactRayOffset;
	//FVector RaycastEndPos = GetActorLocation() - GetActorUpVector() * (groundContactRayOffset + groundContactRayLength);
	//FVector SideRayCastOffset = GetActorRightVector() * groundContactRaySideOffset;

	//FCollisionQueryParams CollisionParams;
	//CollisionParams.AddIgnoredActor(this);

	//FHitResult leftRay, midRay, rightRay;

	//bool checkLHit = GetWorld()->LineTraceSingleByChannel(leftRay, RaycastStartPos - SideRayCastOffset, RaycastEndPos - SideRayCastOffset, ECC_Visibility, CollisionParams);
	//bool checkMHit = GetWorld()->LineTraceSingleByChannel(midRay, RaycastStartPos, RaycastEndPos, ECC_Visibility, CollisionParams);
	//bool checkRHit = GetWorld()->LineTraceSingleByChannel(rightRay, RaycastStartPos + SideRayCastOffset, RaycastEndPos + SideRayCastOffset, ECC_Visibility, CollisionParams);

	//// DrawDebugLines
	//if(checkLHit)
	//	DrawDebugLine(GetWorld(), RaycastStartPos - SideRayCastOffset, RaycastEndPos - SideRayCastOffset, FColor::Green, false);
	//else
	//	DrawDebugLine(GetWorld(), RaycastStartPos - SideRayCastOffset, RaycastEndPos - SideRayCastOffset, FColor::Red, false);

	//if (checkMHit)
	//	DrawDebugLine(GetWorld(), RaycastStartPos, RaycastEndPos, FColor::Green, false);
	//else
	//	DrawDebugLine(GetWorld(), RaycastStartPos, RaycastEndPos, FColor::Red, false);

	//if (checkRHit)
	//	DrawDebugLine(GetWorld(), RaycastStartPos + SideRayCastOffset, RaycastEndPos + SideRayCastOffset, FColor::Green, false);
	//else
	//	DrawDebugLine(GetWorld(), RaycastStartPos + SideRayCastOffset, RaycastEndPos + SideRayCastOffset, FColor::Red, false);

	////Validate contact with surface
	//if (checkMHit)
	//	bWheelContact = true;
	//else if (checkLHit && checkRHit)
	//	bWheelContact = true;
	//else
	//	bWheelContact = false;

	//return bWheelContact;
}

void APlayerPawn::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("BOARD HIT! %s"), *OtherComp->GetName());

	if (*OtherComp->GetName() == FString{ "PointObjectMeshComponent" })
	{
		//OtherActor->Destroy();
		//OtherActor = nullptr;
		//pointsCollected++;
	}
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
	if(ValidateGroundContact())
		PlayerRoot->AddImpulse(GetActorUpVector() * jumpForce);
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
}

