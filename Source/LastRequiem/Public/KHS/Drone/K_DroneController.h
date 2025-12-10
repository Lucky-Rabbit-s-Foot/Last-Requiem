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
	
private:
	void OnDroneLook(const FInputActionValue& value);
	void OnDroneMove(const FInputActionValue& value);
	void OnDroneMoveReleased(const FInputActionValue& value);
	void OnDroneUpDown(const FInputActionValue& value);
	void OnDroneDownReleased(const FInputActionValue& value);
	void OnDroneModeChange(const FInputActionValue& value);
	void OnDroneUseSkill01(const FInputActionValue& value);
	void OnDroneUseSkill02(const FInputActionValue& value);
	
protected:
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
	
	
};
