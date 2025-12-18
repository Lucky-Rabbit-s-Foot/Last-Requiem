// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/UI/K_TutorialWidget.h"

#include "Components/Button.h"

UK_TutorialWidget::UK_TutorialWidget()
{
	UILayer = EUILayer::POPUP;
	zOrder = 99;
}

void UK_TutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveAll(this);
		ExitButton->OnClicked.AddDynamic(this, &UK_TutorialWidget::OnClickedQuitButton);
	}
	
	if (NextButton)
	{
		NextButton->OnClicked.RemoveAll(this);
		NextButton->OnClicked.AddDynamic(this, &UK_TutorialWidget::OnClickedNextImageButton);
	}
	
	if (PrevButton)
	{
		PrevButton->OnClicked.RemoveAll(this);
		PrevButton->OnClicked.AddDynamic(this, &UK_TutorialWidget::OnClickedPreviousImageButton);
	}
}

void UK_TutorialWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UK_TutorialWidget::OnClickedQuitButton()
{
	onCloseUIRequested.Broadcast(this);
}

void UK_TutorialWidget::OnClickedNextImageButton()
{
}

void UK_TutorialWidget::OnClickedPreviousImageButton()
{
}
