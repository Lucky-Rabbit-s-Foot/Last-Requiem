// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "K_BaseUIWidget.h"
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
	//UI레이어에 따라 ZOrder 계산(Persistent : 낮게, Popup : 높게)
	int32 CalculateZOrder(UK_BaseUIWidget* widget) const;
	//현재 Popup 스택 상황에 따라 입력모드 변경
	void NotifyInputModeChange();
	// CloseUI 함수들 닫기 작업 내부 헬퍼함수.
	void CloseUIInternal(UK_BaseUIWidget* widget);
	
public:
	/*
	 * UI를 열고 뷰포트에 추가.
	 * - 이미 열려있어서 캐싱되어있으면 기존 인스턴스 반환
	 * - persistemt UI는 Map에 저장
	 * - popup UI 는 Stack에 추가, 이전 top에 focus lost 알림
	 * @param targetClas : 열고자 하는 UI위젯클래스
	 * @return UI : 열린 UI위젯 인스턴스(실패시 nullptr)
	 */
	template<typename T>
	T* OpenUI(TSubclassOf<T> targetClass);
	
	/*
	 * 위젯을 캐시에서 가져오거나 없으면 생성
	 * - 성능 최적화 : 자주 열리는 UI들의 생성/소멸비용 절감
	 * - 위젯 상태 유지 : 닫았다 다시 열어도 이전 상태 보존
	 * 
	 * @param widgetclass : 가져올/생성할 위젯 클래스
	 * @return 캐싱된, 새로 생성된 위젯 (실패시 nullptr)
	 */
	template<typename T>
	T* GetOrCreateWidget(TSubclassOf<T> widgetClass);
	
	/*
	 * 특정 타입 UI닫고 뷰포트에서 제거(클래스 타입 기반)
	 * - persistent : 맵에서 제거
	 * - popup : 스택 제거 후 새로운 top에게 focusgained 알림
	 * - 참고 : 위젯은 캐시에 계속 보관중이므로 완전히 파괴되지 않음.
	 */
	template<typename T>
	void CloseUI();
	
	/*
	 * 오버로딩 함수. 위젯 인스턴스 직접 받아서 닫을때 사용.
	 * - 위젯 닫기요청 델리게이트 핸들에서 사용.(보통 팝업UI들)
	 * @param widget : 닫고자 하는 위젯 인스턴스
	 */
	void CloseUI(UK_BaseUIWidget* widget);
	/*
	 * 스택 최상위 POP 닫기 (ESC바인딩용)
	 * 스택에서 pop하고 그 아래 팝업에 포커스 전달
	 */
	void CloseTopPopupUI();
	/*
	 * 열러있는 모든 팝업UI 닫기
	 * 게임오버, 씬 전환 시 사용
	 */
	void CloseAllPopupUI();
	/*
	 * 팝업 스택의 새 top에 포커스 전달용
	 */
	void RefreshTopPopupUI();
	
	//팝업이 하나라도 열려있는지 확인
	bool HasOpenPopupUI() const { return popUpUIStack.Num() > 0; }
	
private:
	//현재 열려있는 모든 persistentUI들 맵(클래스->인스턴스)
	UPROPERTY()
	TMap<TSubclassOf<UK_BaseUIWidget>, UK_BaseUIWidget*> persitentUIMap;
	
	//현재 열려있는 모든 popupUI들 스택. Last가 최상위, 포커스 받는 중.
	//나중에 추가된 것이 위에 표시, 입력받음
	UPROPERTY()
	TArray<UK_BaseUIWidget*> popUpUIStack;
	
	//생성된 모든 위젯들 캐싱(클래스->인스턴스)
	//매번 Create하는 거 막고 비용절감
	//UI닫아도 데이터 상태 유지.
	UPROPERTY()
	TMap<TSubclassOf<UK_BaseUIWidget>, UK_BaseUIWidget*> cachedWidgets;
	
};

template<typename T>
T* UK_UIManagerSubsystem::GetOrCreateWidget(TSubclassOf<T> widgetClass)
{
	TSubclassOf<UK_BaseUIWidget> targetClass = widgetClass;
	
	//캐싱된 UI가 있으면 반환
	if (cachedWidgets.Contains(targetClass))
	{
		return Cast<T>(cachedWidgets[targetClass]);
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
		cachedWidgets.Add(targetClass, newWidget);
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
		TSubclassOf<UK_BaseUIWidget> baseClass = targetClass;;
		//Map에 저장된 내용인지 확인 후
		if (!persitentUIMap.Contains(baseClass))
		{
			persitentUIMap.Add(baseClass, baseWidget);
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
void UK_UIManagerSubsystem::CloseUI()
{
	TSubclassOf<T> targetClass = T::StaticClass();
	
	// 캐싱중인 UI라면 리턴
	if (!cachedWidgets.Contains(targetClass))
	{
		return;
	}
	
	UK_BaseUIWidget* widget = cachedWidgets[targetClass];
	
	//실제 닫기 로직은 내부 헬퍼함수 호출
	CloseUIInternal(widget);
}