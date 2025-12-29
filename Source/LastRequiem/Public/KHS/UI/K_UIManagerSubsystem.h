// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "K_BaseUIWidget.h"
#include "KHS/Util/K_LoggingSystem.h"
#include "K_UIManagerSubsystem.generated.h"

/*
 * UI생명주기 및 표시레이어 관리하는 UI서브시스템
 * 
 * 두가지 UI레이어 지원
 * - PERSISTENT : 게임 플레이 중 지속적으로 표시되는 UI(예: HUD, Minimap, UnitList)
 * - POPUP : 필요시 표시되는 팝업 모달 UI(예 : 인벤토리, 설정, 튜토리얼)
 * 
 * 주요기능
 * - 위젯을 캐싱해 반복생성/소멸 비용 절약
 * - Popup UI 스택관리로 팝업 간 포커스 제어
 * - 입력모드 자동 전환 (팝업 Open시 UI ONLY, Close 시 Game Only)
 * 
 */

UCLASS()
class LASTREQUIEM_API UK_UIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
private:
	int32 CalculateZOrder(UK_BaseUIWidget* widget) const;
	void NotifyInputModeChange();
	// CloseUI 함수들 닫기 작업 내부 헬퍼함수.
	void CloseUIInternal(UK_BaseUIWidget* widget);
	
public:
	template<typename T>
	T* OpenUI(TSubclassOf<T> targetClass);
	
	template<typename T>
	T* GetOrCreateWidget(TSubclassOf<T> widgetClass);
	
	template<typename T>
	void CloseUI(TSubclassOf<T> targetClass);
	
	void CloseUI(UK_BaseUIWidget* widget);
	void CloseTopPopupUI();
	void CloseAllPopupUI();
	void RefreshTopPopupUI();
	
	//레벨 전환시 모든UI상태 리셋 함수
	void ResetAllUIStates(); 
	
	FORCEINLINE bool HasOpenPopupUI() const { return popUpUIStack.Num() > 0; }
	FORCEINLINE int GetPopupStackSize() const { return popUpUIStack.Num(); }
	
private:
	//현재 열려있는 Persistent UI들의 맵 (클래스 -> 인스턴스)
	UPROPERTY()
	TMap<TSubclassOf<UK_BaseUIWidget>, UK_BaseUIWidget*> persitentUIMap;
	
	//현재 열려있는 모든 popupUI들 스택. 
	UPROPERTY()
	TArray<UK_BaseUIWidget*> popUpUIStack;
	
	//생성된 모든 위젯들 캐싱(클래스->인스턴스)
	UPROPERTY()
	TMap<TSubclassOf<UK_BaseUIWidget>, UK_BaseUIWidget*> cachedWidgets;
	
public:
	// (20251229) B : 떠있는 위젯찾는 용도
	UFUNCTION ( BlueprintCallable , Category = "UIManager" )
	UK_BaseUIWidget* GetTopPopupUI () const { return popUpUIStack.IsEmpty () ? nullptr : popUpUIStack.Last (); }

};

template<typename T>
T* UK_UIManagerSubsystem::GetOrCreateWidget(TSubclassOf<T> widgetClass)
{
	TSubclassOf<UK_BaseUIWidget> targetClassFactory = widgetClass;
	
	//캐싱된 UI가 있으면 반환
	if (cachedWidgets.Contains(targetClassFactory))
	{
		return Cast<T>(cachedWidgets[targetClassFactory]);
	}
	
	//없으면 생성 후 반환
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (!pc)
	{
		return nullptr;
	}
	
	T* newWidget = CreateWidget<T>(pc, widgetClass);
	if (newWidget)
	{
		cachedWidgets.Add(targetClassFactory, newWidget);
	}
	return newWidget;
}

template <typename T>
T* UK_UIManagerSubsystem::OpenUI(TSubclassOf<T> targetClass)
{
	if (!targetClass)
	{
		return nullptr;
	}
	
	T* widget = GetOrCreateWidget<T>(targetClass);
	if (!widget)
	{
		return nullptr;
	}
	
	if (widget->IsOpen())
	{
		return widget;
	}
	
	UK_BaseUIWidget* baseWidget = widget;
	
	//Persistent 타입 UI일때
	if (baseWidget->UILayer == EUILayer::PERSISTENT)
	{
		TSubclassOf<UK_BaseUIWidget> baseClassFactory = targetClass;
		//Map에 저장된 내용인지 확인 후
		if (!persitentUIMap.Contains(baseClassFactory))
		{
			persitentUIMap.Add(baseClassFactory, baseWidget);
		}
		
		//UI 켜기 및 뷰포트 추가
		baseWidget->OpenUI();
		int32 zOrder = CalculateZOrder(baseWidget);
		baseWidget->AddToViewport(zOrder);
	}
	else //PopUp 타입 UI일때
	{
		//현재의 Top에 FocusLost알림
		if (popUpUIStack.Num() > 0)
		{
			popUpUIStack.Last()->OnFocusLost();
		}
		
		//스택에 추가
		popUpUIStack.Add(baseWidget);
		
		//UI 켜기 및 뷰포트 추가
		baseWidget->OpenUI();
		int32 zOrder = CalculateZOrder(baseWidget);
		baseWidget->AddToViewport(zOrder);
		
		//input모드 변경
		NotifyInputModeChange();
	}
	
	return widget;
}


template<typename T>
void UK_UIManagerSubsystem::CloseUI(TSubclassOf<T> targetClass)
{
	if (!targetClass)
	{
		return;
	}
	
	TSubclassOf<UK_BaseUIWidget> baseClassFactory = targetClass;
	
	// 캐싱중인 UI라면 리턴
	if (!cachedWidgets.Contains(baseClassFactory))
	{
		return;
	}
	
	UK_BaseUIWidget* widget = cachedWidgets[baseClassFactory];
	
	//실제 닫기 로직은 내부 헬퍼함수 호출
	CloseUIInternal(widget);
}