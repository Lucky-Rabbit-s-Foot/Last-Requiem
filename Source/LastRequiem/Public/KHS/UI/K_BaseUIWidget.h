// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "K_BaseUIWidget.generated.h"

//위젯 닫기 요청 공통 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloseUIRequested, UK_BaseUIWidget*, requestingWidget);


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
	
	//위젯에서 닫기 요청 시 사용 델리게이트 
	//BP 이벤트 바인딩 가능
	UPROPERTY(BlueprintAssignable, Category = "LR|UI")
	FOnCloseUIRequested onCloseUIRequested;
	
protected:
	bool bIsOpen = false;
	
public:
	//UI레이어 타입
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	EUILayer UILayer = EUILayer::PERSISTENT;
	
	//뷰포트 표시 순서
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	int32 zOrder = 0;
	
};
