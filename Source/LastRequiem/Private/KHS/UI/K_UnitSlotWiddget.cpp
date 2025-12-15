// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/UI/K_UnitSlotWiddget.h"

#include <string>

#include "KHS/Util/K_LoggingSystem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "KHS/UI/K_UIManagerSubsystem.h"

void UK_UnitSlotWiddget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (SlotButton)
	{
		SlotButton->OnClicked.AddDynamic(this, &UK_UnitSlotWiddget::OnSlotClicked);
	}
	
	SetSlotState(EUnitSlotState::DETECTED);
}

void UK_UnitSlotWiddget::NativeTick(const FGeometry& Geometry, float TimeDelta)
{
	Super::NativeTick(Geometry , TimeDelta);
	
	if (!CanUpdateUIData(TimeDelta))
	{
		return;
	}
}


bool UK_UnitSlotWiddget::CanUpdateUIData(float TimeDelta)
{
	if (currentState != EUnitSlotState::DETECTED)
	{
		return false;
	}
	
	//HP, SP bar 보간
	if (bIsActive && FMath::Abs(currentDisplayedHP - targetDisplayedHP) > 0.1f)
	{
		currentDisplayedHP = FMath::FInterpTo(currentDisplayedHP, targetDisplayedHP, currentDisplayedHP, TimeDelta);
		
		if (HPBar)
		{
			float percent = cachedUnitData.MaxHP > 0? currentDisplayedHP / cachedUnitData.MaxHP : 0.f;
			HPBar->SetPercent(percent);
		}
	}
	
	if (bIsActive && FMath::Abs(currentDisplayedSP - targetDisplayedSP) > 0.1f)
	{
		currentDisplayedSP = FMath::FInterpTo(currentDisplayedSP, targetDisplayedSP, currentDisplayedSP, TimeDelta);
		
		if (SPBar)
		{
			float percent = cachedUnitData.MaxSanity > 0? currentDisplayedSP / cachedUnitData.MaxSanity : 0.f;
			SPBar->SetPercent(percent);
		}
	}
	return true;
}


void UK_UnitSlotWiddget::OnSlotClicked()
{
	if (!bIsActive)
	{
		return;
	}
	
	UK_UIManagerSubsystem* uiManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	if (uiManager && unitDetailPopupUI)
	{
		UK_BaseUIWidget* detailPopUpUI = uiManager->OpenUI<UK_BaseUIWidget>(unitDetailPopupUI);
		
		if (detailPopUpUI)
		{
			//detailPopupUI->SetUnitData
		}
	}
}

void UK_UnitSlotWiddget::UpdateUnitData(const FUnitProfileData& newData)
{
	cachedUnitData = newData;
	targetDisplayedHP = newData.MaxHP;
	targetDisplayedSP = newData.MaxSanity;
	
	if (currentDisplayedHP == 0.0f)
	{
		currentDisplayedHP = targetDisplayedHP;
	}
	
	if (currentDisplayedSP == 0.0f)
	{
		currentDisplayedSP = targetDisplayedSP;
	}
	
	if (!newData.bIsAlive)
	{
		SetSlotState(EUnitSlotState::DEAD);
	}
	
	const FString state = (newData.bIsAlive == true? TEXT("Alive") : TEXT("Dead"));
	KHS_SCREEN_INFO(TEXT("UpdateUnitData Called - HP : %f / %f, SP : %f / %f, State - %s"), 
		currentDisplayedHP, targetDisplayedHP, currentDisplayedSP, targetDisplayedSP, *state);	
}


void UK_UnitSlotWiddget::SetSlotState(EUnitSlotState newState)
{
	currentState = newState;
	UpdateVisualState();
}

void UK_UnitSlotWiddget::UpdateVisualState()
{
	if (!BackgroundImg)
	{
		return;
	}
	
	//EUnitSlotState에 따라 배경이미지 색 변경 / DeatText 가시성 관리
	switch (currentState)
	{
		case EUnitSlotState::DETECTED:
		{
			BackgroundImg->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 1.0f, 0.5f)); 
			if (DeadText)
			{
				DeadText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		break;
	case EUnitSlotState::UNDETECTED:
		{
			BackgroundImg->SetColorAndOpacity(FLinearColor(0.3f, 0.3f, 0.3f, 0.5f)); 
			if (DeadText)
			{
				DeadText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		break;
	case EUnitSlotState::DEAD:
		{
			BackgroundImg->SetColorAndOpacity(FLinearColor(0.5f, 0.0f, 0.0f, 0.5f)); 
			if (DeadText)
			{
				DeadText->SetVisibility(ESlateVisibility::Visible);
			}
		}
		break;
		
	default:
		{
			BackgroundImg->SetColorAndOpacity(FLinearColor(0.3f, 0.3f, 0.3f, 0.5f)); 
			if (DeadText)
			{
				DeadText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		break;
	}
}