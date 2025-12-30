// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BJM/Unit/B_UnitMentalTypes.h"
#include "K_UnitSlotWiddget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitSlotClicked, AActor*, SelectedUnit);

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
	virtual void NativeTick(const FGeometry& Geometry, float TimeDelta) override;
	
	bool CanUpdateUIData(float TimeDelta);

private:
	void UpdateSlotVisualState();
	void SetSlotVisualState(FLinearColor color, ESlateVisibility visibility, bool bIsEnable );
	
protected:
	UFUNCTION()
	void OnSlotClicked();
	
public:
	void UpdateUnitData(const FUnitProfileData& newData);
	void SetSlotState(EUnitSlotState newState);
	void SetLinkedUnit(AActor* targetUnit);
	
	FORCEINLINE AActor* GetLinkedUnit() const { return linkedUnit.Get(); }
	FORCEINLINE EUnitSlotState GetSlotState() const { return currentState; }
	
private:
	bool bIsActive = false;
	bool bIsDead = false;
	bool bIsCombat = false;
	bool bIsSpeaking = false;
	
	FUnitProfileData cachedUnitData;
	
	EUnitSlotState currentState = EUnitSlotState::UNDETECTED;
	
	float currentDisplayedHP = 0.f;
	float targetDisplayedHP = 0.f;
	
	float currentDisplayedSP = 0.f;
	float targetDisplayedSP = 0.f;
	
	const float UI_INTERP_SPEED = 5.0f;
	
	//슬롯과 연결된 유닛
	TWeakObjectPtr<AActor> linkedUnit;
	
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
	
public:
	UPROPERTY(BlueprintAssignable, Category = "LR|Event")
	FOnUnitSlotClicked onSlotClickedDel;
	
// 종민 추가
protected:
	UPROPERTY ( meta = (BindWidgetOptional) )
	class UWidget* SpeechBubbleContainer;

	UPROPERTY ( meta = (BindWidgetOptional) )
	class UTextBlock* MentalStateText;

public:
	UFUNCTION ( BlueprintCallable , Category = "Unit|UI" )
	void SetSpeakingState ( bool InIsSpeaking , FString StateText );

};
