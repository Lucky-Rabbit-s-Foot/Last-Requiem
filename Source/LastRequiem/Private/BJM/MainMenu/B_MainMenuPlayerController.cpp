// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/MainMenu/B_MainMenuPlayerController.h"

void AB_MainMenuPlayerController::BeginPlay ()
{
	Super::BeginPlay ();

	bShowMouseCursor = true;

	FInputModeUIOnly InputModeData;
	InputModeData.SetLockMouseToViewportBehavior ( EMouseLockMode::DoNotLock );

	SetInputMode ( InputModeData );
}
