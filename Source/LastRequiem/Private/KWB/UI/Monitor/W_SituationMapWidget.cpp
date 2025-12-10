// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/Monitor/W_SituationMapWidget.h"
#include "Components/Button.h"

void UW_SituationMapWidget::NativeConstruct()
{
	Super::NativeConstruct ();

	if (Exit)
	{
		Exit->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::OnExitButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Exit이 존재하지 않습니다." ) );
	}

	if (Attack)
	{
		Attack->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::OnAttackButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Attack이 존재하지 않습니다." ) );
	}

	if (Patrol)
	{
		Patrol->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::OnPatrolButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Patrol이 존재하지 않습니다." ) );
	}

	if (Retreat)
	{
		Retreat->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::OnRetreatButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Retreat이 존재하지 않습니다." ) );
	}
}

void UW_SituationMapWidget::OnExitButtonClicked ()
{
	UE_LOG ( LogTemp , Log , TEXT ( "Exit Button Clicked." ) );
	// Widget Hide / 해당 위젯에 해당하는 모든 기능 비활성화 => Not Touchable
}

void UW_SituationMapWidget::OnAttackButtonClicked ()
{
	uint8 orderID = static_cast<uint8>(EOrderButtonType::Attack);
	//HandleOrderButtonClicked ( orderID );
	// TEST
	uint8 TEMP_DelegateReturnVal = HandleOrderButtonClicked ( orderID );
}

void UW_SituationMapWidget::OnPatrolButtonClicked ()
{
	uint8 orderID = static_cast<uint8>(EOrderButtonType::Patrol);
	//HandleOrderButtonClicked ( orderID );
	// TEST
	uint8 TEMP_DelegateReturnVal = HandleOrderButtonClicked ( orderID );
}

void UW_SituationMapWidget::OnRetreatButtonClicked ()
{
	uint8 orderID = static_cast<uint8>(EOrderButtonType::Retreat);
	//HandleOrderButtonClicked ( orderID );
	// TEST
	uint8 TEMP_DelegateReturnVal = HandleOrderButtonClicked ( orderID );
}

// TEST : 실제 코드 리턴 타입은 void
uint8 UW_SituationMapWidget::HandleOrderButtonClicked ( uint8 InOrderID)
{
	uint8 orderType;

	switch (InOrderID)
	{
	case 0:
		UE_LOG ( LogTemp , Log , TEXT ( "Attack Button Clicked." ) );
		orderType = InOrderID;
		break;
	case 1:
		UE_LOG ( LogTemp , Log , TEXT ( "Patrol Button Clicked." ) );
		orderType = InOrderID;
		break;
	case 2:
		UE_LOG ( LogTemp , Log , TEXT ( "Retreat Button Clicked." ) );
		orderType = InOrderID;
		break;
	}

	OnOrderButtonClicked.ExecuteIfBound(orderType);

	return orderType;
}
