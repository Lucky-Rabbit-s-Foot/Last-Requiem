// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/Drone/K_DroneController.h"
#include "KHS/DRone/K_Drone.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

void AK_DroneController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (subsys)
	{
		subsys->AddMappingContext(IMC_Drone, 0);
	}
}

void AK_DroneController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* enhanced = Cast<UEnhancedInputComponent>(InputComponent);
	if (enhanced)
	{
		enhanced->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AK_DroneController::OnDroneLook);
		
		enhanced->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AK_DroneController::OnDroneMove);
		enhanced->BindAction(IA_Move, ETriggerEvent::Completed, this, &AK_DroneController::OnDroneMoveReleased);
		
		enhanced->BindAction(IA_UpDown, ETriggerEvent::Triggered, this, &AK_DroneController::OnDroneUpDown);
		enhanced->BindAction(IA_UpDown, ETriggerEvent::Completed, this, &AK_DroneController::OnDroneDownReleased);
		
		enhanced->BindAction(IA_ModeChange, ETriggerEvent::Started, this, &AK_DroneController::OnDroneModeChange);
		enhanced->BindAction(IA_Skill01, ETriggerEvent::Started, this, &AK_DroneController::OnDroneUseSkill01);
		enhanced->BindAction(IA_Skill02, ETriggerEvent::Started, this, &AK_DroneController::OnDroneUseSkill02);
	}
}


void AK_DroneController::OnDroneLook(const FInputActionValue& value)
{
	FVector2D lookInput = value.Get<FVector2D>();
	lookInput.Y *= -1.f; //요청으로 Y축 입력 반전.
	
	AddYawInput(lookInput.X);
	
	FRotator currentRot = GetControlRotation();
	currentRot.Normalize();
	
	bool bAtMaxLimit = (currentRot.Pitch >= 10.0f && lookInput.Y < 0);
	bool bAtMinLimit = (currentRot.Pitch <= -45.0f && lookInput.Y > 0);
	
	if (!bAtMaxLimit && !bAtMinLimit)
	{
		AddPitchInput(lookInput.Y);
	}
	
	currentRot = GetControlRotation();
	currentRot.Normalize();
	currentRot.Pitch = FMath::Clamp(currentRot.Pitch, -45.0f, 10.0f);
	SetControlRotation(currentRot);
}

void AK_DroneController::OnDroneMove(const FInputActionValue& value)
{
	AK_Drone* drone = Cast<AK_Drone>(GetPawn());
	if (drone)
	{
		drone->Move(value.Get<FVector2D>());
	}
}

void AK_DroneController::OnDroneMoveReleased(const FInputActionValue& value)
{
	AK_Drone* drone = Cast<AK_Drone>(GetPawn());
	if (drone)
	{
		drone->Move(FVector2D::ZeroVector);
	}
}

void AK_DroneController::OnDroneUpDown(const FInputActionValue& value)
{
	AK_Drone* drone = Cast<AK_Drone>(GetPawn());
	if (drone)
	{
		drone->UpDown(value.Get<float>());
	}
}

void AK_DroneController::OnDroneDownReleased(const FInputActionValue& value)
{
	AK_Drone* drone = Cast<AK_Drone>(GetPawn());
	if (drone)
	{
		drone->UpDown(0.0f);
	}
}

void AK_DroneController::OnDroneModeChange(const FInputActionValue& value)
{
	AK_Drone* drone = Cast<AK_Drone>(GetPawn());
	if (drone)
	{
		drone->ChangeMode();
	}
}

void AK_DroneController::OnDroneUseSkill01(const FInputActionValue& value)
{
	AK_Drone* drone = Cast<AK_Drone>(GetPawn());
	if (drone)
	{
		drone->UseSkill01();
	}
}

void AK_DroneController::OnDroneUseSkill02(const FInputActionValue& value)
{
	AK_Drone* drone = Cast<AK_Drone>(GetPawn());
	if (drone)
	{
		drone->UseSkill02();
	}
}