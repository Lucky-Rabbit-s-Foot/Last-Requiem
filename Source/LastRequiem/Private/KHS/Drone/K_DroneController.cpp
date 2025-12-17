// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/Drone/K_DroneController.h"
#include "KHS/Drone/K_Drone.h"

#include "KHS/UI/K_UIManagerSubsystem.h"
#include "KHS/UI/K_BaseUIWidget.h"
#include "KHS/UI/K_HUDWidget.h"
#include "KHS/UI/K_UnitListWidget.h"

#include "KWB/UI/Monitor/W_SituationMapWidget.h"
#include "KWB/UI/W_MapWidget.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "BJM/Unit/B_UnitBase.h"
#include "KHS/Util/K_LoggingSystem.h"

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
		cachedUnitListUI = UIManager->OpenUI<UK_UnitListWidget>(unitListWidget);
		
		BindPersistentUIDelegates();
	}
}

void AK_DroneController::BindPersistentUIDelegates()
{
	if (bPersistentUIBound)
	{
		return;
	}
	
	//캐싱된 UnitList UI의 유닛 선택 델리게이트 구독
	if (cachedUnitListUI)
	{
		cachedUnitListUI->onUnitListSelectedDel.AddDynamic(this, &AK_DroneController::HandleUnitSelected);
	}
	
	bPersistentUIBound = true;
}

void AK_DroneController::BindSituationMapUIDelegates(class UW_SituationMapWidget* situationUI)
{
	if (!situationUI || bSituationMapUIBound)
	{
		return;
	}
	
	cachedSituationUI = situationUI;
	cachedMapWidget =situationUI->GetRenderedMap();
	
	//버튼 델리게이트 연결
	situationUI->OnAttackButtonClicked.AddDynamic(this, &AK_DroneController::HandleAttackButtonClicked);
	situationUI->OnStopButtonClicked.AddDynamic(this, &AK_DroneController::HandleStopButtonClicked);
	situationUI->OnHoldButtonClicked.AddDynamic(this, &AK_DroneController::HandleHoldButtonClicked);
	situationUI->OnRetreatButtonClicked.AddDynamic(this, &AK_DroneController::HandleRetreatButtonClicked);
	
	//맵 델리게이트 연결
	if (cachedMapWidget)
	{
		cachedMapWidget->OnMapUnitSelected.AddDynamic(this, &AK_DroneController::HandleUnitSelected);
		cachedMapWidget->OnMapMoveCommand.AddDynamic(this, &AK_DroneController::HandleMapMoveCommand);
	}
	
	bSituationMapUIBound = true;
}

void AK_DroneController::UnbindPersistentUIDelegates()
{
	if (!bPersistentUIBound)
	{
		return;
	}
	
	//캐싱된 UnitList UI의 유닛 선택 델리게이트 구독 해제
	if (cachedUnitListUI)
	{
		cachedUnitListUI->onUnitListSelectedDel.RemoveDynamic(this, &AK_DroneController::HandleUnitSelected);
	}
	
	cachedUnitListUI = nullptr;
	bPersistentUIBound = false;
}

void AK_DroneController::UnbindSituationMapUIDelegates()
{
	if (!bSituationMapUIBound)
	{
		return;
	}
	
	//버튼 델리게이트 연결
	if (cachedSituationUI)
	{
		cachedSituationUI->OnAttackButtonClicked.RemoveDynamic(this, &AK_DroneController::HandleAttackButtonClicked);
		cachedSituationUI->OnStopButtonClicked.RemoveDynamic(this, &AK_DroneController::HandleStopButtonClicked);
		cachedSituationUI->OnHoldButtonClicked.RemoveDynamic(this, &AK_DroneController::HandleHoldButtonClicked);
		cachedSituationUI->OnRetreatButtonClicked.RemoveDynamic(this, &AK_DroneController::HandleRetreatButtonClicked);
	}
	
	
	//맵 델리게이트 연결
	if (cachedMapWidget)
	{
		cachedMapWidget->OnMapUnitSelected.RemoveDynamic(this, &AK_DroneController::HandleUnitSelected);
		cachedMapWidget->OnMapMoveCommand.RemoveDynamic(this, &AK_DroneController::HandleMapMoveCommand);
	}
	
	cachedSituationUI = nullptr;
	cachedMapWidget = nullptr;
	bSituationMapUIBound = false;
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
		//KHS_SCREEN_INFO(TEXT("열린 미니맵 닫기 - 스택 크기 : %d, 위젯 포인터 : %p"), UIManger->GetPopupStackSize(), minimapUI);
		//이미 열려있으면 닫고 델리게이트 구독 해제
		UnbindSituationMapUIDelegates();
		UIManger->CloseUI(minimapUI);
		minimapUI->onCloseUIRequested.RemoveDynamic(this, &AK_DroneController::HandleUICloseReqeust);
	}
	else
	{
		//KHS_SCREEN_INFO(TEXT("미니맵 새로 열기 - 스택 크기 : %d"), UIManger->GetPopupStackSize());
		//닫혀있으면 열고 델리게이트 구독
		UIManger->OpenUI<UW_SituationMapWidget>(mapWidget);
		minimapUI->onCloseUIRequested.AddDynamic(this, &AK_DroneController::HandleUICloseReqeust);
		BindSituationMapUIDelegates(minimapUI);
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
	//KHS_SCREEN_INFO(TEXT("HandleUICloseRequest is Called"));
	
	if (!requestWidget)
	{
		return;
	}
	
	auto* UIManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (!UIManager)
	{
		return;
	}
	
	//SituaionMap인 경우엔 별도로 델리게이트 구독해제 작업
	UW_SituationMapWidget* situationUi = Cast<UW_SituationMapWidget>(requestWidget);
	if (situationUi)
	{
		UnbindSituationMapUIDelegates();
	}
	
	//인스턴스로 직접 닫는 오버로딩 CloseUI함수 호출
	UIManager->CloseUI(requestWidget);
	
	//델리게이트 구독 해제
	requestWidget->onCloseUIRequested.RemoveDynamic(this, &AK_DroneController::HandleUICloseReqeust);
}

void AK_DroneController::HandleUnitSelected(AActor* selectedActor)
{
	AB_UnitBase* unit = Cast<AB_UnitBase>(selectedActor);
	AB_UnitBase* previousUnit = selectedUnit.Get();
	if (previousUnit && previousUnit != unit)
	{
		previousUnit->SetSelectedSprite(false);
	}
	
	if (unit && unit->IsAlive())
	{
		selectedUnit = unit;
		unit->SetSelectedSprite(true);
		
		if (cachedMapWidget)
		{
			cachedMapWidget->SetSelectedUnit(unit);
		}
		
		KHS_SCREEN_INFO(TEXT("Unit Selected : %s"), *unit->GetName());
		
		//TODO : 선택 피드백 - 하이라이트,  UI업데이트..
	}
	else
	{
		selectedUnit = nullptr;
		if (cachedMapWidget)
		{
			cachedMapWidget->SetSelectedUnit(nullptr);
		}
	}
}

void AK_DroneController::HandleMapMoveCommand(AActor* targetUnit, FVector dest)
{
	AB_UnitBase* unit = Cast<AB_UnitBase>(targetUnit);
	if (!unit || !unit->IsAlive())
	{
		KHS_WARN(TEXT("MoveCommand : No Valid Unit Selected"));
		return;
	}
	
	unit->CommandMoveToLocation(dest);
	KHS_SCREEN_INFO(TEXT("Move Command : %s to %s"), *unit->GetName(), *dest.ToString());
}

void AK_DroneController::HandleAttackButtonClicked()
{
	AB_UnitBase* unit = selectedUnit.Get();
	if (!unit || !unit->IsAlive())
	{
		KHS_WARN(TEXT("Attack : No Valid Unit Selected"));
		return;
	}
	
	unit->OnAttackButtonClicked_Unit();
}

void AK_DroneController::HandleStopButtonClicked()
{
	AB_UnitBase* unit = selectedUnit.Get();
	if (!unit || !unit->IsAlive())
	{
		KHS_WARN(TEXT("Stop : No Valid Unit Selected"));
		return;
	}
	
	unit->OnStopButtonClicked_Unit();
}

void AK_DroneController::HandleHoldButtonClicked()
{
	AB_UnitBase* unit = selectedUnit.Get();
	if (!unit || !unit->IsAlive())
	{
		KHS_WARN(TEXT("Hold : No Valid Unit Selected"));
		return;
	}
	
	unit->OnHoldButtonClicked_Unit();
}

void AK_DroneController::HandleRetreatButtonClicked()
{
	AB_UnitBase* unit = selectedUnit.Get();
	if (!unit || !unit->IsAlive())
	{
		KHS_WARN(TEXT("Retreat : No Valid Unit Selected"));
		return;
	}
	
	unit->OnRetreatButtonClicked_Unit();
}


