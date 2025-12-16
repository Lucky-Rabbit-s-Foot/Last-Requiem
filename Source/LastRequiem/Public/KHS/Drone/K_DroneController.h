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
	//Input Binding Functions
	void OnDroneLook(const FInputActionValue& value);
	void OnDroneMove(const FInputActionValue& value);
	void OnDroneMoveReleased(const FInputActionValue& value);
	void OnDroneUpDown(const FInputActionValue& value);
	void OnDroneDownReleased(const FInputActionValue& value);
	
	void OnDroneUseSkill01(const FInputActionValue& value);
	void OnDroneUseSkill02(const FInputActionValue& value);
	
	void OnOpenSettingUI(const FInputActionValue& value);
	void OnToggleSituationMapUI(const FInputActionValue& value); //드론뷰-상황판 전환용
	
	//UI Management
	void InitializePersistentUI();
	UFUNCTION() //UI 닫기 요청시 공통 델리게이트 핸들러 함수
	void HandleUICloseReqeust(class UK_BaseUIWidget* requestWidget);
	
protected:
	//UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	TSubclassOf<class UK_HUDWidget> hudWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	TSubclassOf<class UK_UnitListWidget> unitListWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	TSubclassOf<class UW_SituationMapWidget> mapWidget;
	
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
	
};
