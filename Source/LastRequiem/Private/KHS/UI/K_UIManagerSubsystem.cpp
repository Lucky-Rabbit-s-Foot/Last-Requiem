// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/UI/K_UIManagerSubsystem.h"
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
		FInputModeUIOnly inputMode;
		inputMode.SetWidgetToFocus(popUpUIStack.Last()->TakeWidget());
		pc->SetInputMode(inputMode);
		pc->SetShowMouseCursor(true);
	}
	else
	{
		pc->SetInputMode(FInputModeGameOnly());
		pc->SetShowMouseCursor(false);
	}
}

void UK_UIManagerSubsystem::CloseTopPopupUI()
{
	//팝업 스택이 없으면 리턴
	if (popUpUIStack.Num() == 0)
	{
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
}
