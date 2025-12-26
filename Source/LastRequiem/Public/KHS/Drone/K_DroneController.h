// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "K_DroneController.generated.h"

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API AK_DroneController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	
private:
	/*
	 * Input Binding Functions Section
	 * - Managing Control of Drone
	 */
	//Movement
	void OnDroneLook(const FInputActionValue& value);
	void OnDroneMove(const FInputActionValue& value);
	void OnDroneMoveReleased(const FInputActionValue& value);
	void OnDroneUpDown(const FInputActionValue& value);
	void OnDroneDownReleased(const FInputActionValue& value);
	//Skills
	void OnDroneUseSkill01(const FInputActionValue& value);
	void OnDroneUseSkill02(const FInputActionValue& value);
	//Play Functions
	void OnOpenSettingUI(const FInputActionValue& value);
	void OnToggleSituationMapUI(const FInputActionValue& value); //드론뷰-상황판 전환용
	
	// (20251226) P : Pause UI (Start)
	void OnOpenPauseUI ( const FInputActionValue& value );
	// (20251226) P : Pause UI (End)

	/*
	 * UI Management FUnctions Section
	 * - Open/Close UI with UI Subsystem
	 * - Handling Request of Widgets
	 * - Binding/Unbinding UI Delegates
	 * - follow up MVC pattern.
	 */
	//UI Management Internal Helpers
	void InitializePersistentUI();
	
	
	//개별 UI 델리게이트 바인딩/언바인딩 헬퍼
	void BindPersistentUIDelegates();
	void UnbindPersistentUIDelegates();
	void BindSituationMapUIDelegates(class UW_SituationMapWidget* situationUI);
	void UnbindSituationMapUIDelegates();
	void BindSettingUIDelegates(class UK_SettingWidget* settingUI);
	void UnbindSettingUIDelegates();
	// (20251226) P : Pause UI (Start)
	void BindPauseUIDelegates ( class UP_PauseWidget* pauseUI );
	void UnbindPauseUIDelegates ();
	// (20251226) P : Pause UI (End)

	
	// (20251224) W : Unit Orders(Keyboard)
	void OnUnitOrderAttack ( const FInputActionValue& value );
	void OnUnitOrderStop ( const FInputActionValue& value );
	void OnUnitOrderHold ( const FInputActionValue& value );
	void OnUnitOrderRetreat ( const FInputActionValue& value );

	//Common
	UFUNCTION() //UI 닫기 요청시 공통 델리게이트 핸들러 함수
	void HandleUICloseRequest(class UK_BaseUIWidget* requestWidget);
	
	//Siutaion Map Widget Handling
	UFUNCTION() //유닛 선택시 공통 델리게이트 핸들러 함수
	void HandleUnitSelected(AActor* selectedActor);
	UFUNCTION()
	void HandleMapMoveCommand(AActor* targetUnit, FVector dest);
	UFUNCTION()
	void HandleUnitAttackButtonClicked();
	UFUNCTION()
	void HandleUnitStopButtonClicked();
	UFUNCTION()
	void HandleUnitHoldButtonClicked();
	UFUNCTION()
	void HandleUnitRetreatButtonClicked();
	
	//Setting Widget Handling
	UFUNCTION()
	void HandleTutorialButtonClicked();
	UFUNCTION()
	void HandleRestartButtonClicked();
	UFUNCTION()
	void HandleQuitGameButtonClicked();

protected:
	//개별 명령 전달 유닛 캐싱
	TWeakObjectPtr<class AB_UnitBase> selectedUnit;
	
	//UI캐싱
	UPROPERTY()
	TObjectPtr<class UK_UnitListWidget> cachedUnitListUI;
	UPROPERTY()
	TObjectPtr<class UW_SituationMapWidget> cachedSituationUI;
	UPROPERTY()
	TObjectPtr<class UW_MapWidget> cachedMiniMapUI;
	UPROPERTY()
	TObjectPtr<class UK_SettingWidget> cachedSettingUI;
	UPROPERTY()
	TObjectPtr<class UK_TutorialWidget> cachedTutorialUI;

	// (20251226) P : Pause UI (Start)
	UPROPERTY ()
	TObjectPtr<class UP_PauseWidget> cachedPauseUI;
	bool bPauseUIBound = false;
	// (20251226) P : Pause UI (End)

	//UI 델리게이트 바인딩 플래그
	bool bPersistentUIBound = false;
	bool bSituationMapUIBound = false;
	bool bSettingUIBound = false;
	
	//UI Classes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	TSubclassOf<class UK_HUDWidget> hudUIFactory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	TSubclassOf<class UK_UnitListWidget> unitListUIFactory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	TSubclassOf<class UW_SituationMapWidget> mapUIFactory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	TSubclassOf<class UK_SettingWidget> settingUIFactory;
	
	// (20251226) P : Pause UI (Start)
	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly , Category = "LR|UI" )
	TSubclassOf<class UP_PauseWidget> pauseUIFactory;
	// (20251226) P : Pause UI (End)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	TSubclassOf<class UK_TutorialWidget> tutorialUIFactory;
	
	//Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputMappingContext> IMC_Drone;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_Move;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_Look;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_UpDown;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_Skill01;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_Skill02;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_ModeChange;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_Setting;

	// (20251224) W : 명령 버튼 키보드 입력 추가 용도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_OrderAttack;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_OrderStop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_OrderHold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_OrderRetreat;
};
