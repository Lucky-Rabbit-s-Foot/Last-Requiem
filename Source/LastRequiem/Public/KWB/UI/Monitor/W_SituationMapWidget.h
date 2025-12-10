// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_SituationMapWidget.generated.h"

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
	void OnMyButtonClicked ();

public:
	// void OnClickActionButtonDelegate ( const FScriptDelegate& Delegate );

protected:
	UPROPERTY ( BlueprintReadOnly , Category = "Widget" , meta = (BindWidget) )
	TObjectPtr<class UW_MapWidget> RenderedMap;

	// 추후 형님 위젯 추가 가능성 고려
	//UPROPERTY ( BlueprintReadOnly , Category = "Widget" , meta = (BindWidget) )
	//TObjectPtr<class UK_UnitStatusWidget> TEMP_UNIT_STATUS;

	UPROPERTY ( BlueprintReadOnly , Category = "Button" , meta = (BindWidget) )
	TObjectPtr<class UButton> Exit = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Attack = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Patrol = nullptr;

	UPROPERTY ( BlueprintReadOnly , Category = "Button|Order" , meta = (BindWidget) )
	TObjectPtr<class UButton> Retreat = nullptr;
};
