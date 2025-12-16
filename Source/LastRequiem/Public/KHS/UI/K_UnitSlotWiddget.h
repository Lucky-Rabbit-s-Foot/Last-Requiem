// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BJM/Unit/B_UnitMentalTypes.h"
#include "K_UnitSlotWiddget.generated.h"

UENUM(BlueprintType)
enum class EUnitSlotState : uint8
{
	DETECTED UMETA(DisplayName = "Detected"),
	UNDETECTED UMETA(DisplayName = "Undetected"),
	DEAD UMETA(DisplayName = "Dead")
};


UCLASS()
class LASTREQUIEM_API UK_UnitSlotWiddget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	bool CanUpdateUIData(float TimeDelta);
	virtual void NativeTick(const FGeometry& Geometry, float TimeDelta) override;
	
private:
	void UpdateVisualState();
	
protected:
	UFUNCTION()
	void OnSlotClicked();
	
public:
	void UpdateUnitData(const FUnitProfileData& newData);

	void SetSlotState(EUnitSlotState newState);
	EUnitSlotState GetSlotState() const { return currentState; }
	
private:
	bool bIsActive = false;
	bool bIsDead = false;
	bool bIsCombat = false;
	bool bIsSpeaking = false;
	
	FUnitProfileData cachedUnitData;
	
	EUnitSlotState currentState = EUnitSlotState::DETECTED;
	
	float currentDisplayedHP = 0.f;
	float targetDisplayedHP = 0.f;
	
	float currentDisplayedSP = 0.f;
	float targetDisplayedSP = 0.f;
	
	const float UI_INTERP_SPEED = 5.0f;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* BackgroundImg;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SlotButton;
	
	UPROPERTY(meta = (BindWidget))
	class UImage* ProfileImage;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* UnitName;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SPBar;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DeadText;
	
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class UK_BaseUIWidget> unitDetailPopupUI;
	
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	float lerpSpeed = 5.f;
	
	
};
