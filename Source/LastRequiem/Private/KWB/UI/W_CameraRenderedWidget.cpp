// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/W_CameraRenderedWidget.h"

void UW_CameraRenderedWidget::NativeConstruct ()
{
	Super::NativeConstruct ();

	// TEST: TEMP Func
	if (GetOwningPlayer ())
	{
		GetOwningPlayer ()->bShowMouseCursor = true;
	}
}
