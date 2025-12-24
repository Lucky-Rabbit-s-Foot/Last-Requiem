// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_DroneSkillWidget.generated.h"

class UProgressBar;
class UBorder;
class AK_Drone;
/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UW_DroneSkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION ( BlueprintCallable , Category = "DroneSkill" )
	void InitWithDrone ( AK_Drone* InDrone );

protected:
	virtual void NativeConstruct () override;
	virtual void NativeDestruct () override;

	void UpdateDroneUI ();

	void UpdateSkillSlot (
		int32 SkillIndex ,
		bool bReadyNow ,
		float CooldownRatioRemaining ,
		UBorder* Border ,
		UProgressBar* ProgressBar ,
		UTexture2D* ReadyIcon ,
		UTexture2D* CooldownIcon ,
		bool& bLastCoolingDownState
	);

	void SetBorderIcon ( UBorder* Border , UTexture2D* Tex );

private:
	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<UBorder> Skill01Border;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<UProgressBar> Skill01PGBar;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<UBorder> Skill02Border;

	UPROPERTY ( meta = (BindWidget) )
	TObjectPtr<UProgressBar> Skill02PGBar;

	UPROPERTY ( EditAnywhere , Category = "DroneSkill|Visual" )
	TObjectPtr<UTexture2D> Skill01_ReadyIcon;

	UPROPERTY ( EditAnywhere , Category = "DroneSkill|Visual" )
	TObjectPtr<UTexture2D> Skill01_CooldownIcon;

	UPROPERTY ( EditAnywhere , Category = "DroneSkill|Visual" )
	TObjectPtr<UTexture2D> Skill02_ReadyIcon;

	UPROPERTY ( EditAnywhere , Category = "DroneSkill|Visual" )
	TObjectPtr<UTexture2D> Skill02_CooldownIcon;

private:
	TWeakObjectPtr<AK_Drone> Drone;

	FTimerHandle UIUpdateTimerHandle;

	// 아이콘 스왑을 매 프레임 하지 않기 위해 상태 기억
	bool bSkill01CoolingDownLast = false;
	bool bSkill02CoolingDownLast = false;
};
