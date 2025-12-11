// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_SituationMapWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnOrderButtonClicked );

// 추후 시스템 변경 대응용
//UENUM ( BlueprintType )
//enum class EOrderButtonType : uint8
//{
//	Attack ,
//	Patrol ,
//	Retreat
//};
/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UW_SituationMapWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct () override;

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

	// 추후 시스템 변경 대응용
	//UFUNCTION ()
	//void HandleOrderButtonClicked ( uint8 InOrderID );

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

	// FOnOrderButtonClicked OnOrderButtonClicked; // HandleOrderButtonClicked 대응용

protected:
	UPROPERTY ( BlueprintReadOnly , Category = "Widget" , meta = (BindWidget) )
	TObjectPtr<class UW_MapWidget> RenderedMap;

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
