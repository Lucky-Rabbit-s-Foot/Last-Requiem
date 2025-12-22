// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/MainMenu/B_MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void UB_MainMenuWidget::OnClickNewGame ()
{
	// 맵전환
	// UGameplayStatics::OpenLevel ( this , FName ( "맵 이름" ) );
}

void UB_MainMenuWidget::OnClickExitGame ()
{
	UKismetSystemLibrary::QuitGame ( this , GetOwningPlayer () , EQuitPreference::Quit , true );
}
