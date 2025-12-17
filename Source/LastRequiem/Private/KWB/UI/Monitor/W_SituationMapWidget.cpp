// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/Monitor/W_SituationMapWidget.h"
#include "Components/Button.h"
#include "KHS/Util/K_LoggingSystem.h"

UW_SituationMapWidget::UW_SituationMapWidget ()
{
	UILayer = EUILayer::POPUP;
	zOrder = 0;
}

void UW_SituationMapWidget::NativeConstruct()
{
	Super::NativeConstruct ();

	if (Exit)
	{
		Exit->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleExitButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Exit이 존재하지 않습니다." ) );
	}

	if (Attack)
	{
		Attack->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleAttackButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Attack이 존재하지 않습니다." ) );
	}

	if (Stop)
	{
		Stop->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleStopButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Stop이 존재하지 않습니다." ) );
	}

	if (Hold)
	{
		Hold->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleHoldButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Hold이 존재하지 않습니다." ) );
	}

	if (Retreat)
	{
		Retreat->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleRetreatButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Retreat이 존재하지 않습니다." ) );
	}
}

void UW_SituationMapWidget::NativeDestruct ()
{
	if (Exit)
	{
		Exit->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleExitButtonClicked );
		//KHS_INFO ( TEXT ( "Exit->OnClicked.RemoveDynamic" ) );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Exit이 존재하지 않습니다." ) );
	}

	if (Attack)
	{
		Attack->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleAttackButtonClicked );
		//KHS_INFO ( TEXT ( "Attack->OnClicked.RemoveDynamic" ) );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Attack이 존재하지 않습니다." ) );
	}

	if (Stop)
	{
		Stop->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleStopButtonClicked );
		//KHS_INFO ( TEXT ( "Stop->OnClicked.RemoveDynamic" ) );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Stop이 존재하지 않습니다." ) );
	}

	if (Hold)
	{
		Hold->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleHoldButtonClicked );
		//KHS_INFO ( TEXT ( "Hold->OnClicked.RemoveDynamic" ) );

	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Hold이 존재하지 않습니다." ) );
	}

	if (Retreat)
	{
		Retreat->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleRetreatButtonClicked );
		KHS_INFO ( TEXT ( "Retreat->OnClicked.RemoveDynamic" ) );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Retreat이 존재하지 않습니다." ) );
	}
}

void UW_SituationMapWidget::HandleExitButtonClicked ()
{
	UE_LOG ( LogTemp , Log , TEXT ( "Exit Button Clicked." ) );
	onCloseUIRequested.Broadcast ( this );
}

void UW_SituationMapWidget::HandleAttackButtonClicked ()
{
	OnAttackButtonClicked.Broadcast ();
	UE_LOG ( LogTemp , Log , TEXT ( "Attack Button Clicked." ) );
}

void UW_SituationMapWidget::HandleStopButtonClicked ()
{
	OnStopButtonClicked.Broadcast ();
	UE_LOG ( LogTemp , Log , TEXT ( "Stop Button Clicked." ) );
}

void UW_SituationMapWidget::HandleHoldButtonClicked ()
{
	OnHoldButtonClicked.Broadcast ();
	UE_LOG ( LogTemp , Log , TEXT ( "Hold Button Clicked." ) );
}

void UW_SituationMapWidget::HandleRetreatButtonClicked ()
{
	OnRetreatButtonClicked.Broadcast ();
	UE_LOG ( LogTemp , Log , TEXT ( "Retreat Button Clicked." ) );
}