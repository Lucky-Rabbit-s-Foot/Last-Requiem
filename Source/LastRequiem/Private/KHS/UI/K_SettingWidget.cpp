// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/UI/K_SettingWidget.h"
#include "Components/Button.h"

UK_SettingWidget::UK_SettingWidget()
{
	UILayer = EUILayer::POPUP;
	zOrder = 10;
}

void UK_SettingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (TutorialButton)
	{
		TutorialButton->OnClicked.RemoveAll(this);
		TutorialButton->OnClicked.AddDynamic(this, &UK_SettingWidget::OnClickedTutorialButton);
	}
	
	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &UK_SettingWidget::OnClickedBackButton);
	}
	
	if (RestartButton)
	{
		RestartButton->OnClicked.RemoveAll(this);
		RestartButton->OnClicked.AddDynamic(this, &UK_SettingWidget::OnClickedRestartButton);
	}
	
	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveAll(this);
		QuitButton->OnClicked.AddDynamic(this, &UK_SettingWidget::OnClickedQuitButton);
	}
}

void UK_SettingWidget::NativeDestruct()
{
	
	Super::NativeDestruct();
}

void UK_SettingWidget::OnClickedTutorialButton()
{
	onTutorialRequestedDel.Broadcast();
}

void UK_SettingWidget::OnClickedBackButton()
{
	onCloseUIRequested.Broadcast(this);
}

void UK_SettingWidget::OnClickedRestartButton()
{
	onRestartRequestedDel.Broadcast();
}

void UK_SettingWidget::OnClickedQuitButton()
{
	onQuitGameRequestedDel.Broadcast();
}
