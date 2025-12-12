// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/Drone/K_DroneController.h"
#include "KHS/Drone/K_Drone.h"

#include "KHS/UI/K_UIManagerSubsystem.h"
#include "KHS/UI/K_BaseUIWidget.h"
#include "KHS/UI/K_HUDWidget.h"
#include "KHS/UI/K_UnitListWidget.h"

#include "KWB/UI/Monitor/W_SituationMapWidget.h"

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
		
		enhanced->BindAction(IA_ModeChange, ETriggerEvent::Started, this, &AK_DroneController::OnToggleSituationMapUI);
		enhanced->BindAction(IA_Setting, ETriggerEvent::Started, this, &AK_DroneController::OnOpenSettingUI);
		
		enhanced->BindAction(IA_Skill01, ETriggerEvent::Started, this, &AK_DroneController::OnDroneUseSkill01);
		enhanced->BindAction(IA_Skill02, ETriggerEvent::Started, this, &AK_DroneController::OnDroneUseSkill02);
	}
}

void AK_DroneController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	InitializePersistentUI();
}


void AK_DroneController::InitializePersistentUI()
{
	UK_UIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (UIManager)
	{
		UIManager->OpenUI<UK_HUDWidget>(hudWidget);
		UIManager->OpenUI<UK_UnitListWidget>(unitListWidget);
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

void AK_DroneController::OnToggleSituationMapUI(const FInputActionValue& value)
{
	if (!mapWidget)
	{
		return;
	}
	
	auto* UIManger = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (!UIManger)
	{
		return;
	}
	
	auto* minimapUI = UIManger->GetOrCreateWidget<UW_SituationMapWidget>(mapWidget);
	if (!minimapUI)
	{
		return;
	}
	
	if (minimapUI->IsOpen())
	{
		//이미 열려있으면 닫고 델리게이트 구독 해제
		UIManger->CloseUI<UW_SituationMapWidget>();
		minimapUI->onCloseUIRequested.RemoveDynamic(this, &AK_DroneController::HandleUICloseReqeust);
	}
	else
	{
		//닫혀있으면 열고 델리게이트 구독
		UIManger->OpenUI<UW_SituationMapWidget>(mapWidget);
		minimapUI->onCloseUIRequested.AddDynamic(this, &AK_DroneController::HandleUICloseReqeust);
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

void AK_DroneController::OnOpenSettingUI(const FInputActionValue& value)
{
	
}


void AK_DroneController::HandleUICloseReqeust(class UK_BaseUIWidget* requestWidget)
{
	if (!requestWidget)
	{
		return;
	}
	
	auto* UIManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (!UIManager)
	{
		return;
	}
	
	//인스턴스로 직접 닫는 오버로딩 CloseUI함수 호출
	UIManager->CloseUI(requestWidget);
	
	//델리게이트 구독 해제
	requestWidget->onCloseUIRequested.RemoveDynamic(this, &AK_DroneController::HandleUICloseReqeust);
}


