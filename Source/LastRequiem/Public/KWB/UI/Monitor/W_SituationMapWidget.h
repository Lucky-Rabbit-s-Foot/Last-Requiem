// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/UI/K_BaseUIWidget.h"
#include "W_SituationMapWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnOrderButtonClicked );

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UW_SituationMapWidget : public UK_BaseUIWidget
{
	GENERATED_BODY()

private:
	UW_SituationMapWidget ();

public:
	inline class UW_MapWidget* GetRenderedMap () const { return RenderedMap; }

protected:
	virtual void NativeConstruct () override;

	virtual void NativeDestruct () override;

	UFUNCTION ()
	void HandleExitButtonClicked ();

	UFUNCTION ()
	void HandleAttackButtonClicked ();

	UFUNCTION ()
	void HandleStopButtonClicked ();

	UFUNCTION ()
	void HandleHoldButtonClicked ();

	UFUNCTION ()
	void HandleRetreatButtonClicked ();

public:
	// 공격(Attack) 버튼 클릭 시
	UPROPERTY ( BlueprintAssignable , Category = "Unit|Event" )
	FOnOrderButtonClicked OnAttackButtonClicked;

	// 정지(Stop) 버튼 클릭 시
	UPROPERTY ( BlueprintAssignable , Category = "Unit|Event" )
	FOnOrderButtonClicked OnStopButtonClicked;

	// 홀드(Hold) 버튼 클릭 시
	UPROPERTY ( BlueprintAssignable , Category = "Unit|Event" )
	FOnOrderButtonClicked OnHoldButtonClicked;

	// 후퇴(Retreat) 버튼 클릭 시
	UPROPERTY ( BlueprintAssignable , Category = "Unit|Event" )
	FOnOrderButtonClicked OnRetreatButtonClicked;

protected:
	UPROPERTY ( BlueprintReadOnly , Category = "Widget" , meta = (BindWidget) )
	TObjectPtr<UW_MapWidget> RenderedMap;

	UPROPERTY ( BlueprintReadOnly , Category = "Button" , meta = (BindWidget) )
	TObjectPtr<class UButton> Exit = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Attack = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Stop = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Hold = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Retreat = nullptr;
};
