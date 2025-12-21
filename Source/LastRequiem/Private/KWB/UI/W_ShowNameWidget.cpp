// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/W_ShowNameWidget.h"
#include "BJM/Unit/B_UnitBase.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "KHS/Util/K_LoggingSystem.h"

void UW_ShowNameWidget::NativeConstruct ()
{
	Super::NativeConstruct ();

	SetVisibility ( ESlateVisibility::HitTestInvisible );
}

void UW_ShowNameWidget::InitializeOwner ( AActor* InOwnerUnit )
{
	OwnerUnit = InOwnerUnit;

	if (OwnerUnit.IsValid ())
	{
		SetUnitNameText ( OwnerUnit.Get () );
		KHS_INFO ( TEXT ( "ShowName: InitializeOwner -> %s" ) , *OwnerUnit->GetName () );
	}
}

void UW_ShowNameWidget::SetUnitNameText ( AActor* InOwnerUnit )
{
	if (!UnitNameText || !IsValid ( InOwnerUnit ))
	{
		return;
	}

	// 에디터에선 Label, 런타임에선 Name 쪽으로 안전하게 표시
	const FString DisplayName = InOwnerUnit->GetActorNameOrLabel ();
	UnitNameText->SetText ( FText::FromString ( DisplayName ) );
}

void UW_ShowNameWidget::SetShowNameState ( EUnitShowNameState NewState , bool bPlayPopup )
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
	ApplyVisibility ( bPlayPopup );
}

void UW_ShowNameWidget::ApplyVisibility ( bool bPlayPopup )
{
	const bool bShouldShow = (CurrentState == EUnitShowNameState::DETECTED);

	// 클릭/히트테스트를 막지 않게 하고 싶으면 HitTestInvisible 추천
	SetVisibility ( bShouldShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed );

	if (bShouldShow && bPlayPopup)
	{
		PlayPopupAnimation ();
	}
}

void UW_ShowNameWidget::PlayPopupAnimation ()
{
	if (PopupAnimation)
	{
		PlayAnimation ( PopupAnimation );
	}
}

void UW_ShowNameWidget::ToggleShowNameWidget ()
{
	const ESlateVisibility Cur = GetVisibility ();
	const bool bIsShown =
		(Cur != ESlateVisibility::Collapsed) &&
		(Cur != ESlateVisibility::Hidden);

	SetVisibility ( bIsShown ? ESlateVisibility::Collapsed
		: ESlateVisibility::HitTestInvisible );
}
