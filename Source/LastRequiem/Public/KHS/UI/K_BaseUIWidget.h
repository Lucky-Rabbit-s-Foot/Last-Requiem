// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "K_BaseUIWidget.generated.h"


UENUM(BlueprintType)
enum class EUILayer : uint8
{
	PERSISTENT UMETA(DisplayName = "Persistent"),
	POPUP UMETA(DisplayName = "Popup"),
};

UCLASS()
class LASTREQUIEM_API UK_BaseUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void OpenUI();
	virtual void CloseUI();
	virtual void RefreshUI();
	virtual void OnFocusGained();
	virtual void OnFocusLost();
	
	FORCEINLINE bool IsOpen() const { return bIsOpen; }
	
protected:
	bool bIsOpen = false;
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	EUILayer UILayer = EUILayer::PERSISTENT;
	
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	int32 zOrder = 0;
	
};
