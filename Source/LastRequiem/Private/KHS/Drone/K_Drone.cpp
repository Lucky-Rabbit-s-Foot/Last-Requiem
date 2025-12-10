// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/Drone/K_Drone.h"
#include "KHS/Data/K_DroneData.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"



// Sets default values
AK_Drone::AK_Drone()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(100.f);
	
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(RootComponent);
	cameraComp->SetRelativeLocation(FVector(0.0f, -3.0f, 20.0f));
	cameraComp->bUsePawnControlRotation = true;
	
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>("meshComp");
	meshComp->SetupAttachment(RootComponent);
	
	
	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AK_Drone::BeginPlay()
{
	Super::BeginPlay();
	
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsys)
		{
			subsys->AddMappingContext(IMC_Drone, 0);
		}
	}
	
	//SFX settings
	
	//UI Settings
	// if (droneUIFactory)
	// {
	// 	droneUI = CreateWidget<UUserWidget>(GetWorld(), droneUIFactory);
	// 	if (droneUI)
	// 	{
	// 		droneUI->AddToViewPort(0);
	// 		
	// 	}
	// }
}

// Called every frame
void AK_Drone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDroneSpeed(DeltaTime);
}

// Called to bind functionality to input
void AK_Drone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhanced = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhanced)
	{
		enhanced->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AK_Drone::OnDroneMove);
		enhanced->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AK_Drone::OnDroneLook);
		enhanced->BindAction(IA_UpDown, ETriggerEvent::Triggered, this, &AK_Drone::OnDroneUpDown);
		enhanced->BindAction(IA_Scan, ETriggerEvent::Started, this, &AK_Drone::OnDroneScan);
		enhanced->BindAction(IA_ModeChange, ETriggerEvent::Started, this, &AK_Drone::OnDroneModeChange);
	}
}

void AK_Drone::OnDroneLook(const FInputActionValue& value)
{
	FVector2D lookInput = value.Get<FVector2D>();
	
	if (Controller)
	{
		AddControllerYawInput(lookInput.X);
		AddControllerPitchInput(lookInput.Y);
	}
}

void AK_Drone::OnDroneMove(const FInputActionValue& value)
{
	moveInputValue = value.Get<FVector2D>();
}

void AK_Drone::OnDroneUpDown(const FInputActionValue& value)
{
	upDownInputValue = value.Get<float>();
}

void AK_Drone::OnDroneScan(const FInputActionValue& value)
{
}

void AK_Drone::OnDroneModeChange(const FInputActionValue& value)
{
}

void AK_Drone::UpdateDroneSpeed(float DeltaTime)
{
	FVector forwardDir = GetActorForwardVector();
	FVector rightDir = GetActorRightVector();
	FVector upDir = FVector::UpVector;
	
	FVector horizontalTargetVec = (forwardDir * moveInputValue.Y + rightDir * moveInputValue.X) * droneData->DRONE_MAX_SPEED;
	FVector verticalTargetVec = upDir * upDownInputValue * droneData->DRONE_MAX_VERTICAL_SPEED;
	
	targetVelocity = horizontalTargetVec + verticalTargetVec;
	
	float interpSpeed;
	if (moveInputValue.SizeSquared() > 0.0f || FMath::Abs(upDownInputValue) > 0.0f)
	{
		interpSpeed = droneData->ACCELERATION;
	}
	else
	{
		interpSpeed = droneData->DECELERATION;
	}
	
	curVelocity = FMath::VInterpTo(curVelocity, targetVelocity, DeltaTime, interpSpeed);
	
	FVector newLoc = GetActorLocation() + curVelocity * DeltaTime;
	SetActorLocation(newLoc);
}

void AK_Drone::UpdateDroneAltitude()
{
}

