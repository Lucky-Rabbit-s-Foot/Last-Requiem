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
	//자식 클래스에서 오버라이드하여 데이터 갱신 로직 구현
}

void UK_BaseUIWidget::OnFocusGained()
{
	//자식 클래스에서 오버라이드 하여 포커스 들어왔을때 로직 구현
}

void UK_BaseUIWidget::OnFocusLost()
{
	//자식 클래스에서 오버라이드 하여 포커스 잃을때 로직 구현
}