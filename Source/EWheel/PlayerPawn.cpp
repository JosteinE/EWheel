// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
//#include "Engine/SkeletalMesh.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/// Car mesh
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("/Game/Vehicle/Sedan/Sedan_SkelMesh.Sedan_SkelMesh"));
	//GetMesh()->SetSkeletalMesh(CarMesh.Object);

	//static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Vehicle/Sedan/Sedan_AnimBP"));
	//GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Root component. Seperate from the mesh to avoid problems when rotating/tilting the board
	//PlayerRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PlayerRootComponent"));
	//RootComponent = PlayerRoot;

	// Player Mesh
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Meshes/TempPlayerWheel.TempPlayerWheel'"));
	if(MeshAsset.Succeeded())
		GetMesh()->SetStaticMesh(MeshAsset.Object);
	PlayerMesh->SetCollisionProfileName("Pawn");
	PlayerMesh->OnComponentHit.AddDynamic(this, &APlayerPawn::OnMeshHit);
	RootComponent = PlayerMesh;

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 60.f);
	SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
	SpringArm->SetupAttachment(PlayerMesh);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 2.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 7.f;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	// Auto possess this pawn
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	AutoReceiveInput = EAutoReceiveInput::Player0;
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


	// Simulate physics if the board is not colliding
	if (!ValidateGroundContact() && !PlayerMesh->IsSimulatingPhysics())
		PlayerMesh->SetSimulatePhysics(true);
	else if (bWheelContact && PlayerMesh->IsSimulatingPhysics() && bIsCollidingWithGround)
		PlayerMesh->SetSimulatePhysics(false);
	bIsCollidingWithGround = false;

	if(!PlayerMesh->IsSimulatingPhysics())
	{
		//Move the actor based on input
		MoveBoard(DeltaTime);
		//Rotate the actor based on input
		SetActorRotation(GetActorRotation() + FRotator{ 0, movementInput.X, 0 } *turnSpeed * DeltaTime);
		//Tilt the board in the direction of movement
		BoardTilt(DeltaTime);
	}
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerPawn::MoveRight);
}

void APlayerPawn::MoveBoard(float DeltaTime)
{
	FVector forwardDirection = GetActorForwardVector();
	forwardDirection.Z = 0;
	SetActorLocation(GetActorLocation() + forwardDirection * GetClaculatedSpeed(DeltaTime), false);
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

	FRotator newRotation = GetMesh()->GetRelativeRotation();
	newRotation.Pitch = -currentBoardTilt.Pitch;
	newRotation.Roll = currentBoardTilt.Roll;

	GetMesh()->SetRelativeRotation(newRotation);
}

bool APlayerPawn::ValidateGroundContact()
{
	FVector RaycastStartPos = GetActorLocation() - GetActorUpVector() * groundContactRayOffset;
	FVector RaycastEndPos = GetActorLocation() - GetActorUpVector() * (groundContactRayOffset + groundContactRayLength);
	FVector SideRayCastOffset = GetActorRightVector() * groundContactRaySideOffset;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FHitResult leftRay, midRay, rightRay;

	bool checkLHit = GetWorld()->LineTraceSingleByChannel(leftRay, RaycastStartPos - SideRayCastOffset, RaycastEndPos - SideRayCastOffset, ECC_Visibility, CollisionParams);
	bool checkMHit = GetWorld()->LineTraceSingleByChannel(midRay, RaycastStartPos, RaycastEndPos, ECC_Visibility, CollisionParams);
	bool checkRHit = GetWorld()->LineTraceSingleByChannel(rightRay, RaycastStartPos + SideRayCastOffset, RaycastEndPos + SideRayCastOffset, ECC_Visibility, CollisionParams);

	// DrawDebugLines
	if(checkLHit)
		DrawDebugLine(GetWorld(), RaycastStartPos - SideRayCastOffset, RaycastEndPos - SideRayCastOffset, FColor::Green, false);
	else
		DrawDebugLine(GetWorld(), RaycastStartPos - SideRayCastOffset, RaycastEndPos - SideRayCastOffset, FColor::Red, false);

	if (checkMHit)
		DrawDebugLine(GetWorld(), RaycastStartPos, RaycastEndPos, FColor::Green, false);
	else
		DrawDebugLine(GetWorld(), RaycastStartPos, RaycastEndPos, FColor::Red, false);

	if (checkRHit)
		DrawDebugLine(GetWorld(), RaycastStartPos + SideRayCastOffset, RaycastEndPos + SideRayCastOffset, FColor::Green, false);
	else
		DrawDebugLine(GetWorld(), RaycastStartPos + SideRayCastOffset, RaycastEndPos + SideRayCastOffset, FColor::Red, false);

	//Validate contact with surface
	if (checkMHit)
		bWheelContact = true;
	else if (checkLHit && checkRHit)
		bWheelContact = true;
	else
		bWheelContact = false;

	return bWheelContact;
}

void APlayerPawn::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("BOARD HIT!"), bIsCollidingWithGround);
	bIsCollidingWithGround = Hit.bBlockingHit;
}

void APlayerPawn::MoveForward(float input)
{
	movementInput.Y = input;
}

void APlayerPawn::MoveRight(float input)
{
	movementInput.X = input;
}

