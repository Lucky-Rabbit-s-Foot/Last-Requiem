// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/Monitor/W_SituationMapWidget.h"
#include "Components/Button.h"

void UW_SituationMapWidget::NativeConstruct()
{
	Super::NativeConstruct ();

	if (Exit)
	{
		// TEST : 실 코드에서는 Exit에는 연결 X
		Exit->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::OnMyButtonClicked );
	}
}

void UW_SituationMapWidget::OnMyButtonClicked ()
{
}
