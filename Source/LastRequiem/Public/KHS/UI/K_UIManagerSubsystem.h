// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "K_BaseUIWidget.h"
#include "K_UIManagerSubsystem.generated.h"


UCLASS()
class LASTREQUIEM_API UK_UIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
private:
	UK_BaseUIWidget* GetOrCreateWidget(TSubclassOf<UK_BaseUIWidget> targetClass);
	
	int32 CalculateZOrder(UK_BaseUIWidget* widget) const;
	void NotifyInputModeChange();
	
public:
	template<typename T>
	T* OpenUI(TSubclassOf<T> targetClass);
	
	void CloseTopPopupUI();
	void CloseAllPopupUI();
	void RefreshTopPopupUI();
	
	template<typename T>
	void CloseUI();
	
	bool HasOpenPopupUI() const { return popUpUIStack.Num() > 0; }
	
private:
	UPROPERTY()
	TMap<TSubclassOf<UK_BaseUIWidget>, UK_BaseUIWidget*> persitentUIMap;
	
	UPROPERTY()
	TArray<UK_BaseUIWidget*> popUpUIStack;
	
	UPROPERTY()
	TMap<TSubclassOf<UK_BaseUIWidget>, UK_BaseUIWidget*> cachedWidgets;
	
};

template <typename T>
T* UK_UIManagerSubsystem::OpenUI(TSubclassOf<T> targetClass)
{
	if (!targetClass)
	{
		return nullptr;
	}
	
	UK_BaseUIWidget* widget = GetOrCreateWidget(targetClass);
	if (!widget)
	{
		return nullptr;
	}
	
	if (widget->IsOpen())
	{
		return Cast<T>(widget);
	}
	
	//Persistent 타입 UI일때
	if (widget->UILayer == EUILayer::PERSISTENT)
	{
		//Map에 저장된 내용인지 확인 후
		if (!persitentUIMap.Contains(targetClass))
		{
			persitentUIMap.Add(targetClass, widget);
		}
		
		//UI 켜기 및 뷰포트 추가
		widget->OpenUI();
		int32 zOrder = CalculateZOrder(widget);
		widget->AddToViewport(zOrder);
	}
	else //PopUp 타입 UI일때
	{
		//현재의 Top에 FocusLost알림
		if (popUpUIStack.Num() > 0)
		{
			popUpUIStack.Last()->OnFocusLost();
		}
		
		//스택에 추가
		popUpUIStack.Add(widget);
		
		//UI 켜기 및 뷰포트 추가
		widget->OpenUI();
		int32 zOrder = CalculateZOrder(widget);
		widget->AddToViewport(zOrder);
		
		//input모드 변경
		NotifyInputModeChange();
	}
	
	return Cast<T>(widget);
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
	if (!widget || !widget->IsOpen())
	{
		return;
	}
	
	//Persistent 타입 UI일때
	if (widget->UILayer == EUILayer::PERSISTENT)
	{
		widget->CloseUI();
		widget->RemoveFromParent();
		persitentUIMap.Remove(targetClass);
	}
	else //PopUp 타입 UI일때
	{
		//스택에서 제거
		int32 idx = popUpUIStack.Find(targetClass);
		if (idx != INDEX_NONE)
		{
			popUpUIStack.RemoveAt(idx);
			widget->CloseUI();
			widget->RemoveFromParent();
			
			//새로운 Top Refresh
			RefreshTopPopupUI();
			NotifyInputModeChange();
		}
	}
}