// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/UI/K_BaseUIWidget.h"

void UK_BaseUIWidget::OpenUI()
{
	bIsOpen = true;
	SetVisibility(ESlateVisibility::Visible);
}

void UK_BaseUIWidget::CloseUI()
{
	bIsOpen = false;
	SetVisibility(ESlateVisibility::Hidden);
}

void UK_BaseUIWidget::RefreshUI()
{
	
}

void UK_BaseUIWidget::OnFocusGained()
{
	
}

void UK_BaseUIWidget::OnFocusLost()
{
	
}