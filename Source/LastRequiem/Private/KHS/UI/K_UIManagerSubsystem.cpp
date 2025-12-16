// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/UI/K_UIManagerSubsystem.h"
#include "KHS/Util/K_LoggingSystem.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UK_UIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UK_UIManagerSubsystem::Deinitialize()
{
	CloseAllPopupUI();
	
	for (auto& pair : cachedWidgets)
	{
		if (pair.Value && pair.Value->IsInViewport())
		{
			pair.Value->RemoveFromParent();
		}
	}
	
	cachedWidgets.Empty();
	persitentUIMap.Empty();
	popUpUIStack.Empty();
	
	Super::Deinitialize();
}



int32 UK_UIManagerSubsystem::CalculateZOrder(UK_BaseUIWidget* widget) const
{
	if (!widget)
	{
		return 0;
	}
	
	//Persistent 타입의 경우 ZOrder 낮게
	if (widget->UILayer == EUILayer::PERSISTENT)
	{
		return widget->zOrder;
	}
	//PopUP타입의 경우 ZOrder 높게
	else
	{
		return 100 + popUpUIStack.Num();
	}
}

void UK_UIManagerSubsystem::NotifyInputModeChange()
{
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (!pc)
	{
		return;
	}
	
	if (HasOpenPopupUI())
	{
		//popup있을떈 ui only모드
		FInputModeGameAndUI inputModeGameAndUI;
		FInputModeUIOnly inputMode;
		inputMode.SetWidgetToFocus(popUpUIStack.Last()->TakeWidget());
		pc->SetInputMode(inputModeGameAndUI);
		pc->SetShowMouseCursor(true);
	}
	else
	{
		pc->SetInputMode(FInputModeGameOnly());
		pc->SetShowMouseCursor(false);
	}
}

void UK_UIManagerSubsystem::CloseUIInternal(UK_BaseUIWidget* widget)
{
	if (!widget || !widget->IsOpen())
	{
		KHS_INFO(TEXT("CloseUIInternal에 widget이 들어오지 않았음"));
		return;
	}
	
	//Persistent 타입 UI일때
	if (widget->UILayer == EUILayer::PERSISTENT)
	{
		KHS_SCREEN_INFO(TEXT("PERSISTENT 타입 UI 제거"));
		
		widget->CloseUI();
		widget->RemoveFromParent();
		
		TSubclassOf<UK_BaseUIWidget> widgetClass = widget->GetClass();
		persitentUIMap.Remove(widgetClass);
	}
	else //PopUp 타입 UI일때
	{
		KHS_SCREEN_INFO(TEXT("POPUP 타입 UI 제거"));
		//스택에서 제거
		int32 idx = popUpUIStack.Find(widget);
		if (idx != INDEX_NONE)
		{
			popUpUIStack.RemoveAt(idx);
			widget->CloseUI();
			widget->RemoveFromParent();
			
			KHS_SCREEN_INFO(TEXT("위젯 제거됨 - IsInViewport: %s, IsVisible: %s"), 
				widget->IsInViewport() ? TEXT("TRUE") : TEXT("FALSE"),
				widget->IsVisible() ? TEXT("TRUE") : TEXT("FALSE"));
			
			//새로운 Top Refresh
			RefreshTopPopupUI();
			NotifyInputModeChange();
		}
		else
		{
			KHS_SCREEN_INFO(TEXT("popupStack에 해당 UI없음!"));
		}
	}
}

void UK_UIManagerSubsystem::CloseUI(UK_BaseUIWidget* widget)
{
	//인스턴스 타입으로 닫을땐 내부 헬퍼함수로 위임
	CloseUIInternal(widget);
}

void UK_UIManagerSubsystem::CloseTopPopupUI()
{
	//팝업 스택이 없으면 리턴
	if (popUpUIStack.Num() == 0)
	{
		KHS_INFO(TEXT("마지막 팝업 스택"));
		return;
	}
	
	UK_BaseUIWidget* topWidget = popUpUIStack.Pop();
	topWidget->CloseUI();
	topWidget->RemoveFromParent();
	
	RefreshTopPopupUI();
	NotifyInputModeChange();
}

void UK_UIManagerSubsystem::CloseAllPopupUI()
{
	while (popUpUIStack.Num() > 0)
	{
		CloseTopPopupUI();
	}
}

void UK_UIManagerSubsystem::RefreshTopPopupUI()
{
	//스택에 팝업이 남아있다면
	if (popUpUIStack.Num() > 0)
	{
		//새로운 Top에게 Focus알림
		popUpUIStack.Last()->OnFocusGained();
	}
}
