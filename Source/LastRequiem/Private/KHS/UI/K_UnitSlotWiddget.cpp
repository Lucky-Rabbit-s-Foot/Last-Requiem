// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/UI/K_UnitSlotWiddget.h"

#include <string>

#include "BJM/Unit/B_UnitBase.h"
#include "KHS/Util/K_LoggingSystem.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Dataflow/DataflowMathNodes.h"
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
		bIsActive = false;
		
		return false;
	}
	
	//HP, SP bar 보간 (Active일때만 업데이트 작동)
	if (bIsActive)
	{
		currentDisplayedHP = FMath::FInterpTo(currentDisplayedHP, targetDisplayedHP, TimeDelta, UI_INTERP_SPEED);
		
		if (HPBar)
		{
			float percent = cachedUnitData.MaxHP > 0? currentDisplayedHP / cachedUnitData.MaxHP : 0.f;
			HPBar->SetPercent(percent);
		}
		
		currentDisplayedSP = FMath::FInterpTo(currentDisplayedSP, targetDisplayedSP, TimeDelta , UI_INTERP_SPEED );
		
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
	
	//연결된 유닛이 있을때 클릭 델리게이트 브로드캐스트
	if (linkedUnit.IsValid())
	{
		AB_UnitBase* unit = Cast<AB_UnitBase>(linkedUnit.Get());
		if (unit && unit->IsAlive())
		{
			onSlotClickedDel.Broadcast(linkedUnit.Get());
		}
	}
	
	//TODO : 상세 팝업UI를 만들어서 유닛별 상태 보여주기 추가 구현 시 
	// UK_UIManagerSubsystem* uiManager = GetGameInstance()->GetSubsystem<UK_UIManagerSubsystem>();
	// if (uiManager && unitDetailPopupUI)
	// {
	// 	UK_BaseUIWidget* detailPopUpUI = uiManager->OpenUI<UK_BaseUIWidget>(unitDetailPopupUI);
	// 	
	// 	if (detailPopUpUI)
	// 	{
	// 		//detailPopupUI->SetUnitData
	// 	}
	// }
}

void UK_UnitSlotWiddget::UpdateUnitData(const FUnitProfileData& newData)
{
	cachedUnitData = newData;
	
	bIsActive = true;
	
	if (UnitName)
	{
		UnitName->SetText(newData.UnitName);
	}
	
	if (ProfileImage && newData.ProfileImage)
	{
		ProfileImage->SetBrushFromTexture(newData.ProfileImage);
	}
	
	//목표값은 Unit의 가장 최신 수치 반영
	targetDisplayedHP = newData.CurrentHP;
	targetDisplayedSP = newData.CurrentSanity;
	
	//첫 초기화 시에만 즉시 설정
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
}


void UK_UnitSlotWiddget::SetSlotState(EUnitSlotState newState)
{
	currentState = newState;
	
	UpdateSlotVisualState();
}

void UK_UnitSlotWiddget::SetLinkedUnit(AActor* targetUnit)
{
	linkedUnit = targetUnit;
}

void UK_UnitSlotWiddget::UpdateSlotVisualState()
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
			SetSlotVisualState(FLinearColor(0.0f, 0.0f, 1.0f, 0.5f), ESlateVisibility::Hidden, true);
		}
		break;
	case EUnitSlotState::UNDETECTED:
		{
			SetSlotVisualState(FLinearColor(0.3f, 0.3f, 0.3f, 0.5f), ESlateVisibility::Hidden, true);
		}
		break;
	case EUnitSlotState::DEAD:
		{
			SetSlotVisualState(FLinearColor(0.5f, 0.0f, 0.0f, 0.5f), ESlateVisibility::Visible, false);
		}
		break;
		
	default:
		{
			SetSlotVisualState(FLinearColor(0.3f, 0.3f, 0.3f, 0.5f), ESlateVisibility::Hidden, false);
		}
		break;
	}
}

void UK_UnitSlotWiddget::SetSlotVisualState(FLinearColor color, ESlateVisibility visibility, bool bIsEnable)
{
	BackgroundImg->SetColorAndOpacity(color); 
	
	if (DeadText)
	{
		DeadText->SetVisibility(visibility);
	}
	
	if (SlotButton)
	{
		SlotButton->SetIsEnabled(bIsEnable);	
	}
}
