// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/Drone/K_DroneController.h"
#include "KHS/Drone/K_Drone.h"
#include "KHS/Util/K_LoggingSystem.h"

#include "KHS/UI/K_UIManagerSubsystem.h"
#include "KHS/UI/K_BaseUIWidget.h"
#include "KHS/UI/K_HUDWidget.h"
#include "KHS/UI/K_UnitListWidget.h"
#include "KHS/UI/K_SettingWidget.h"
#include "KWB/UI/W_MapWidget.h"

#include "KWB/UI/Monitor/W_SituationMapWidget.h"
#include "BJM/Unit/B_UnitBase.h"
#include "PJB/Pause/P_PauseWidget.h"
#include "PJB/LevelSelector/P_PauseLevelSelector.h"
#include "PJB/Pause/P_TutorialAlbum.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "KHS/UI/K_TutorialWidget.h"
#include "Kismet/GameplayStatics.h"

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
		// enhanced->BindAction(IA_Setting, ETriggerEvent::Started, this, &AK_DroneController::OnOpenSettingUI);
		// (20251226) P : Pause UI (Start)
		enhanced->BindAction(IA_Setting, ETriggerEvent::Started, this, &AK_DroneController::OnOpenPauseUI);
		// (20251226) P : Pause UI (End)
		
		enhanced->BindAction(IA_Skill01, ETriggerEvent::Started, this, &AK_DroneController::OnDroneUseSkill01);
		enhanced->BindAction(IA_Skill02, ETriggerEvent::Started, this, &AK_DroneController::OnDroneUseSkill02);

		// (20251224) W : Orders (Keyboard)
		enhanced->BindAction ( IA_OrderAttack , ETriggerEvent::Started , this , &AK_DroneController::OnUnitOrderAttack );
		enhanced->BindAction ( IA_OrderStop , ETriggerEvent::Started , this , &AK_DroneController::OnUnitOrderStop );
		enhanced->BindAction ( IA_OrderHold , ETriggerEvent::Started , this , &AK_DroneController::OnUnitOrderHold );
		enhanced->BindAction ( IA_OrderRetreat , ETriggerEvent::Started , this , &AK_DroneController::OnUnitOrderRetreat );
	}
}

void AK_DroneController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	InitializePersistentUI();
	
	OpenTutorialUI();
}


void AK_DroneController::InitializePersistentUI()
{
	//KHS_SCREEN_INFO(TEXT("==== Initialize Persistent UI START! ===="));
	
	UK_UIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (UIManager)
	{
		UIManager->OpenUI<UK_HUDWidget>(hudUIFactory);
		cachedUnitListUI = UIManager->OpenUI<UK_UnitListWidget>(unitListUIFactory);
		
		auto* hud = UIManager->GetOrCreateWidget<UK_HUDWidget>(hudUIFactory);
		//KHS_SCREEN_INFO(TEXT("HUD isOpen : %s, ISInViewport : %s"), hud->IsOpen()? TEXT("TRUE") : TEXT("False"), hud->IsInViewport()? TEXT("TRUE") : TEXT("FASLE"));
		
		BindPersistentUIDelegates();
	}
	
	//KHS_SCREEN_INFO(TEXT("==== Initialize Persistent UI END! ===="));
}

void AK_DroneController::OpenTutorialUI()
{
	if (!tutorialUIFactory)
	{
		KHS_WARN(TEXT("Tutorial : No Valid Widget"));
		return;
	}
	
	auto* UIManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (!UIManager)
	{
		KHS_WARN(TEXT("UIManager : No Valid Instance"));
		return;
	}
	
	SetPause(true);
	
	auto* tutorialUI = UIManager->OpenUI<UP_TutorialAlbum>(tutorialUIFactory);
	if (tutorialUI)
	{
		//닫기 델리게이트 구독
		tutorialUI->onCloseUIRequested.AddDynamic(this, &AK_DroneController::HandleUICloseRequest);
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
	cachedMiniMapUI =situationUI->GetRenderedMap();
	
	//버튼 델리게이트 연결
	situationUI->OnAttackButtonClicked.AddDynamic(this, &AK_DroneController::HandleUnitAttackButtonClicked);
	situationUI->OnStopButtonClicked.AddDynamic(this, &AK_DroneController::HandleUnitStopButtonClicked);
	situationUI->OnHoldButtonClicked.AddDynamic(this, &AK_DroneController::HandleUnitHoldButtonClicked);
	situationUI->OnRetreatButtonClicked.AddDynamic(this, &AK_DroneController::HandleUnitRetreatButtonClicked);
	
	//맵 델리게이트 연결
	if (cachedMiniMapUI)
	{
		cachedMiniMapUI->OnMapUnitSelected.AddDynamic(this, &AK_DroneController::HandleUnitSelected);
		cachedMiniMapUI->OnMapMoveCommand.AddDynamic(this, &AK_DroneController::HandleMapMoveCommand);
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
		cachedSituationUI->OnAttackButtonClicked.RemoveDynamic(this, &AK_DroneController::HandleUnitAttackButtonClicked);
		cachedSituationUI->OnStopButtonClicked.RemoveDynamic(this, &AK_DroneController::HandleUnitStopButtonClicked);
		cachedSituationUI->OnHoldButtonClicked.RemoveDynamic(this, &AK_DroneController::HandleUnitHoldButtonClicked);
		cachedSituationUI->OnRetreatButtonClicked.RemoveDynamic(this, &AK_DroneController::HandleUnitRetreatButtonClicked);
	}
	
	//맵 델리게이트 연결
	if (cachedMiniMapUI)
	{
		cachedMiniMapUI->OnMapUnitSelected.RemoveDynamic(this, &AK_DroneController::HandleUnitSelected);
		cachedMiniMapUI->OnMapMoveCommand.RemoveDynamic(this, &AK_DroneController::HandleMapMoveCommand);
	}
	
	cachedSituationUI = nullptr;
	cachedMiniMapUI = nullptr;
	bSituationMapUIBound = false;
}

void AK_DroneController::BindSettingUIDelegates(class UK_SettingWidget* settingUI)
{
	if (!settingUI || bSituationMapUIBound)
	{
		KHS_INFO(TEXT("NO Valid Setting Ui or is already bound with Controller"));
		return;
	}
	
	cachedSettingUI = settingUI;
	
	settingUI->onTutorialRequestedDel.AddDynamic(this, &AK_DroneController::HandleTutorialButtonClicked);
	settingUI->onRestartRequestedDel.AddDynamic(this, &AK_DroneController::HandleRestartButtonClicked);
	settingUI->onQuitGameRequestedDel.AddDynamic(this, &AK_DroneController::HandleQuitGameButtonClicked);
	
	bSettingUIBound = true;
}

void AK_DroneController::UnbindSettingUIDelegates()
{
	if (!bSituationMapUIBound || !cachedSettingUI)
	{
		KHS_INFO(TEXT("NO Valid cached Ui or is already unbound with Controller"));
		return;
	}
	
	cachedSettingUI->onTutorialRequestedDel.RemoveDynamic(this, &AK_DroneController::HandleTutorialButtonClicked);
	cachedSettingUI->onRestartRequestedDel.RemoveDynamic(this, &AK_DroneController::HandleRestartButtonClicked);
	cachedSettingUI->onQuitGameRequestedDel.RemoveDynamic(this, &AK_DroneController::HandleQuitGameButtonClicked);
	
	cachedSettingUI = nullptr;
	bSettingUIBound = false;
}

// (20251226) P : Pause UI (Start)
void AK_DroneController::BindPauseUIDelegates ( UP_PauseWidget* pauseUI )
{
	if (!pauseUI || bSituationMapUIBound)
	{
		KHS_INFO ( TEXT ( "NO Valid Setting Ui or is already bound with Controller" ) );
		return;
	}

	cachedPauseUI = pauseUI;

	pauseUI->onRestartRequestedDel.AddDynamic ( this , &AK_DroneController::HandleRestartButtonClicked );
	pauseUI->onQuitGameRequestedDel.AddDynamic ( this , &AK_DroneController::HandleQuitGameButtonClicked );

	bPauseUIBound = true;
}
void AK_DroneController::UnbindPauseUIDelegates ()
{
	if (!bSituationMapUIBound || !cachedPauseUI)
	{
		KHS_INFO ( TEXT ( "NO Valid cached Ui or is already unbound with Controller" ) );
		return;
	}

	cachedPauseUI->LevelSelector->onRestartRequestedDel.RemoveDynamic ( this , &AK_DroneController::HandleRestartButtonClicked );
	cachedPauseUI->LevelSelector->onQuitGameRequestedDel.RemoveDynamic ( this , &AK_DroneController::HandleQuitGameButtonClicked );
	
	cachedPauseUI = nullptr;
	bPauseUIBound = false;
}
// (20251226) P : Pause UI (End)

// (20251224) W : Unit Order(Keyboard)
void AK_DroneController::OnUnitOrderAttack ( const FInputActionValue& value )
{
	(void)value;
	HandleUnitAttackButtonClicked ();
}

void AK_DroneController::OnUnitOrderStop ( const FInputActionValue& value )
{
	(void)value;
	HandleUnitStopButtonClicked ();
}

void AK_DroneController::OnUnitOrderHold ( const FInputActionValue& value )
{
	(void)value;
	HandleUnitHoldButtonClicked ();
}

void AK_DroneController::OnUnitOrderRetreat ( const FInputActionValue& value )
{
	(void)value;
	HandleUnitRetreatButtonClicked ();
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
	if (!mapUIFactory)
	{
		KHS_WARN(TEXT("No Valid SituationMap Widget"));
		return;
	}
	
	auto* UIManger = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (!UIManger)
	{
		KHS_WARN(TEXT("No Valid UI Subsystem"));
		return;
	}
	
	auto* minimapUI = UIManger->GetOrCreateWidget<UW_SituationMapWidget>(mapUIFactory);
	
	if (!minimapUI)
	{
		KHS_WARN(TEXT("No Valid SituationMap Class"));
		return;
	}
	
	if (minimapUI->IsOpen())
	{
		//KHS_SCREEN_INFO(TEXT("열린 미니맵 닫기 - 스택 크기 : %d, 위젯 포인터 : %p"), UIManger->GetPopupStackSize(), minimapUI);
		//이미 열려있으면 닫고 델리게이트 구독 해제
		UnbindSituationMapUIDelegates();
		UIManger->CloseUI(minimapUI);
		minimapUI->onCloseUIRequested.RemoveDynamic(this, &AK_DroneController::HandleUICloseRequest);
	}
	else
	{
		//KHS_SCREEN_INFO(TEXT("미니맵 새로 열기 - 스택 크기 : %d"), UIManger->GetPopupStackSize());
		//닫혀있으면 열고 델리게이트 구독
		UIManger->OpenUI<UW_SituationMapWidget>(mapUIFactory);
		minimapUI->onCloseUIRequested.AddDynamic(this, &AK_DroneController::HandleUICloseRequest);
		BindSituationMapUIDelegates(minimapUI);
	}
}

void AK_DroneController::OnOpenPauseUI ( const FInputActionValue& value )
{
	auto* UIManager = GetGameInstance ()->GetSubsystem<UK_UIManagerSubsystem> ();
	if (!UIManager)
	{
		KHS_WARN ( TEXT ( "No Valid UI Subsystem" ) );
		return;
	}

	if (UIManager->HasOpenPopupUI ())
	{

		UK_BaseUIWidget* TopWidget = UIManager->GetTopPopupUI ();
		if (TopWidget)
		{
			HandleUICloseRequest ( TopWidget );
		}
	}
	else
	{
		if (!pauseUIFactory)
		{
			KHS_WARN ( TEXT ( "No Valid PauseUI Factory" ) );
			return;
		}

		auto* pauseUI = UIManager->GetOrCreateWidget<UP_PauseWidget> ( pauseUIFactory );
		if (pauseUI)
		{
			if (pauseUI->IsOpen ()) return;

			SetPause ( true );

			UIManager->OpenUI<UP_PauseWidget> ( pauseUIFactory );
			pauseUI->onCloseUIRequested.AddDynamic ( this , &AK_DroneController::HandleUICloseRequest );
			BindPauseUIDelegates ( pauseUI );
		}
	}


	//if (!pauseUIFactory)
	//{
	//	KHS_WARN ( TEXT ( "No Valid SettingWidget" ) );
	//	return;
	//}

	//auto* UIManager = GetGameInstance ()->GetSubsystem<UK_UIManagerSubsystem> ();
	//if (!UIManager)
	//{
	//	KHS_WARN ( TEXT ( "No Valid UI Subsystem" ) );
	//	return;
	//}

	//auto* pauseUI = UIManager->GetOrCreateWidget<UP_PauseWidget> ( pauseUIFactory );
	//if (!pauseUI)
	//{
	//	KHS_WARN ( TEXT ( "No Valid Casted Widget" ) );
	//	return;
	//}

	//if (pauseUI->IsOpen ())
	//{
	//	return;
	//}

	//SetPause ( true );

	//UIManager->OpenUI<UP_PauseWidget> ( pauseUIFactory );
	//pauseUI->onCloseUIRequested.AddDynamic ( this , &AK_DroneController::HandleUICloseRequest );
	//BindPauseUIDelegates ( pauseUI );
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
	if (!settingUIFactory)
	{
		KHS_WARN(TEXT("No Valid SettingWidget"));
		return;
	}
	
	auto* UIManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (!UIManager)
	{
		KHS_WARN(TEXT("No Valid UI Subsystem"));
		return;
	}
	
	auto* settingUI = UIManager->GetOrCreateWidget<UK_SettingWidget>(settingUIFactory);
	if (!settingUI)
	{
		KHS_WARN(TEXT("No Valid Casted Widget"));
		return;
	}
	
	if (settingUI->IsOpen())
	{
		return;
	}
	
	SetPause(true);
	
	UIManager->OpenUI<UK_SettingWidget>(settingUIFactory);
	
	settingUI->onCloseUIRequested.AddDynamic(this, &AK_DroneController::HandleUICloseRequest);
	
	BindSettingUIDelegates(settingUI);
}

void AK_DroneController::HandleUICloseRequest(class UK_BaseUIWidget* requestWidget)
{
	if (!requestWidget)
	{
		KHS_WARN(TEXT("No Valid RequestWidget for close"));
		return;
	}
	
	auto* UIManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (!UIManager)
	{
		KHS_WARN(TEXT("No Valid UI Subsystem"));
		return;
	}
	
	//SituaionMap인 경우엔 별도로 델리게이트 구독해제 작업
	UW_SituationMapWidget* situationUi = Cast<UW_SituationMapWidget>(requestWidget);
	if (situationUi)
	{
		UnbindSituationMapUIDelegates();
	}
	
	//SettingUI의 경우 델리게이트 구독해제 작업
	UK_SettingWidget* settingUI = Cast<UK_SettingWidget>(requestWidget);
	if (settingUI)
	{
		UnbindSettingUIDelegates();
		//게임 재개
		SetPause(false);
	}
	
	// (20251226) P : Pause UI (Start)
	UP_PauseWidget* pauseUI = Cast<UP_PauseWidget> ( requestWidget );
	if (pauseUI)
	{
		UnbindPauseUIDelegates ();
		//게임 재개
		SetPause ( false );
	}
	// (20251226) P : Pause UI (End)

	//인스턴스로 직접 닫는 오버로딩 CloseUI함수 호출
	UIManager->CloseUI(requestWidget);
	
	//델리게이트 구독 해제
	requestWidget->onCloseUIRequested.RemoveDynamic(this, &AK_DroneController::HandleUICloseRequest);
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
		
		if (cachedMiniMapUI)
		{
			cachedMiniMapUI->SetSelectedUnit(unit);
		}
		
		KHS_SCREEN_INFO(TEXT("Unit Selected : %s"), *unit->GetName());
		
		//TODO : 선택 피드백 - 하이라이트,  UI업데이트..
	}
	else
	{
		selectedUnit = nullptr;
		if (cachedMiniMapUI)
		{
			cachedMiniMapUI->SetSelectedUnit(nullptr);
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

void AK_DroneController::HandleUnitAttackButtonClicked()
{
	AB_UnitBase* unit = selectedUnit.Get();
	if (!unit || !unit->IsAlive())
	{
		KHS_WARN(TEXT("Attack : No Valid Unit Selected"));
		return;
	}
	
	unit->OnAttackButtonClicked_Unit();
}

void AK_DroneController::HandleUnitStopButtonClicked()
{
	AB_UnitBase* unit = selectedUnit.Get();
	if (!unit || !unit->IsAlive())
	{
		KHS_WARN(TEXT("Stop : No Valid Unit Selected"));
		return;
	}
	
	unit->OnStopButtonClicked_Unit();
}

void AK_DroneController::HandleUnitHoldButtonClicked()
{
	AB_UnitBase* unit = selectedUnit.Get();
	if (!unit || !unit->IsAlive())
	{
		KHS_WARN(TEXT("Hold : No Valid Unit Selected"));
		return;
	}
	
	unit->OnHoldButtonClicked_Unit();
}

void AK_DroneController::HandleUnitRetreatButtonClicked()
{
	AB_UnitBase* unit = selectedUnit.Get();
	if (!unit || !unit->IsAlive())
	{
		KHS_WARN(TEXT("Retreat : No Valid Unit Selected"));
		return;
	}
	
	unit->OnRetreatButtonClicked_Unit();
}

void AK_DroneController::HandleTutorialButtonClicked()
{
	if (!tutorialUIFactory)
	{
		KHS_WARN(TEXT("Tutorial : No Valid Widget"));
		return;
	}
	
	auto* UIManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (!UIManager)
	{
		KHS_WARN(TEXT("UIManager : No Valid Instance"));
		return;
	}
	
	auto* tutorialUI = UIManager->OpenUI<UP_TutorialAlbum>(tutorialUIFactory);
	if (tutorialUI)
	{
		//닫기 델리게이트 구독
		tutorialUI->onCloseUIRequested.AddDynamic(this, &AK_DroneController::HandleUICloseRequest);
	}
}

void AK_DroneController::HandleRestartButtonClicked()
{
	auto* UIManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (!UIManager)
	{
		KHS_WARN(TEXT("UIManager : No Valid Instance"));
		return;
	}
	
	UIManager->CloseAllPopupUI();
	UIManager->ResetAllUIStates();
	
	//게임 재개
	SetPause(false);
	
	//레벨 재시작
	//-> UIManager에서 OnLevelChanged 호출되어 UI초기화 진행
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName(), false));
}

void AK_DroneController::HandleQuitGameButtonClicked()
{
	//게임 종료
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}


