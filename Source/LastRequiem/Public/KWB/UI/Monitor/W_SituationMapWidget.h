// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_SituationMapWidget.generated.h"

DECLARE_DELEGATE_OneParam ( FOnOrderButtonClicked, uint8 );

UENUM ( BlueprintType )
enum class EOrderButtonType : uint8
{
	Attack ,
	Patrol ,
	Retreat
};
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
	void OnExitButtonClicked ();

	UFUNCTION ()
	void OnAttackButtonClicked ();

	UFUNCTION ()
	void OnPatrolButtonClicked ();

	UFUNCTION ()
	void OnRetreatButtonClicked ();

	UFUNCTION ()
	uint8 HandleOrderButtonClicked ( uint8 InOrderID ); // TEST : 실제 코드 리턴 타입은 void

public:
	FOnOrderButtonClicked OnOrderButtonClicked;

protected:
	UPROPERTY ( BlueprintReadOnly , Category = "Widget" , meta = (BindWidget) )
	TObjectPtr<class UW_MapWidget> RenderedMap;

	UPROPERTY ( BlueprintReadOnly , Category = "Button" , meta = (BindWidget) )
	TObjectPtr<class UButton> Exit = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Attack = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Patrol = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Retreat = nullptr;
};
