// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BJM/Unit/B_UnitMentalTypes.h"
#include "W_ShowNameWidget.generated.h"

UENUM ( BlueprintType )
enum class EUnitShowNameState : uint8
{
	DETECTED UMETA ( DisplayName = "Detected" ) ,
	UNDETECTED UMETA ( DisplayName = "Undetected" ) ,
};


UCLASS()
class LASTREQUIEM_API UW_ShowNameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION ( BlueprintCallable , Category = "UI" )
	void InitializeOwner ( AActor* InOwnerUnit );

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetUnitNameText (AActor* InOwnerUnit);

	UFUNCTION ( BlueprintCallable , Category = "UI" )
	void SetShowNameState ( EUnitShowNameState NewState , bool bPlayPopup = true );

	UFUNCTION ( BlueprintCallable , Category = "UI" )
	void PlayPopupAnimation();

	UFUNCTION ( BlueprintCallable , Category = "UI" )
	void ToggleShowNameWidget ();

protected:
	virtual void NativeConstruct () override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> UnitNameText = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> UnitImage = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> PopupAnimation = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "UI" )
	EUnitShowNameState InitialState = EUnitShowNameState::UNDETECTED;

private:
	void ApplyVisibility ( bool bPlayPopup );

private:
	TWeakObjectPtr<AActor> OwnerUnit;
	EUnitShowNameState CurrentState = EUnitShowNameState::UNDETECTED;
};
