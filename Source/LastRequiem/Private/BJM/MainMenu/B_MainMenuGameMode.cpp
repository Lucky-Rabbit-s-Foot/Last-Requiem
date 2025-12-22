// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/MainMenu/B_MainMenuGameMode.h"
#include "BJM/MainMenu/B_MainMenuPlayerController.h"

AB_MainMenuGameMode::AB_MainMenuGameMode ()
{
	DefaultPawnClass = nullptr;

	PlayerControllerClass = AB_MainMenuPlayerController::StaticClass ();

}
