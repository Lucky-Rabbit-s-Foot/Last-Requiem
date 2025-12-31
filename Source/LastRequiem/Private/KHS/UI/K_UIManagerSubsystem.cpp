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
	//widget 이름이 뭔지 로그	로 남기기
	KHS_INFO ( TEXT ( "CloseUIInternal 호출된 widget : %s" ) , widget ? *widget->GetName () : TEXT ( "nullptr" ) );

	if (!widget || !widget->IsOpen())
	{
		KHS_INFO(TEXT("CloseUIInternal에 widget이 들어오지 않았음"));
		return;
	}
	
	//Persistent 타입 UI일때
	if (widget->UILayer == EUILayer::PERSISTENT)
	{
		
		widget->CloseUI();
		widget->RemoveFromParent();
		
		TSubclassOf<UK_BaseUIWidget> widgetClass = widget->GetClass();
		persitentUIMap.Remove(widgetClass);
	}
	else //PopUp 타입 UI일때
	{
		//스택에서 제거
		int32 idx = popUpUIStack.Find(widget);
		if (idx != INDEX_NONE)
		{
			popUpUIStack.RemoveAt(idx);
			widget->CloseUI();
			widget->RemoveFromParent();
			
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

void UK_UIManagerSubsystem::ResetAllUIStates()
{
	//KHS_SCREEN_INFO(TEXT("==== OnLevelChanged START! ===="));
	
	//캐싱된 인스턴스들 상태 초기화
	for (auto& pair : cachedWidgets)
	{
		if (pair.Value)
		{
			if (pair.Value->IsOpen())
			{
				pair.Value->CloseUI();
			}
			
			if (pair.Value->IsInViewport())
			{
				pair.Value->RemoveFromParent();
			}
		}
	}
	 
	//관련 컨테이너 비우기
	persitentUIMap.Empty();
	popUpUIStack.Empty();
	cachedWidgets.Empty ();
	
	//입력모드 초기화
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->SetInputMode(FInputModeGameOnly());
		pc->SetShowMouseCursor(false);
	}
	
	//KHS_SCREEN_INFO(TEXT("==== OnLevelChanged END! ===="));
}
