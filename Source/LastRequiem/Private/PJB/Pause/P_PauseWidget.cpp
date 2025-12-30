#include "PJB/Pause/P_PauseWidget.h"

#include "PJB/Pause/P_TutorialAlbum.h"
#include "PJB/LevelSelector/P_PauseLevelSelector.h"

UP_PauseWidget::UP_PauseWidget ()
{
	UILayer = EUILayer::POPUP;
	zOrder = 10;
}



//void UP_PauseWidget::NativeConstruct ()
//{
//	Super::NativeConstruct ();
//
//	if (APlayerController* PC = GetOwningPlayer ())
//	{
//		PC->SetShowMouseCursor ( true );
//
//		FInputModeUIOnly InputModeData;
//		InputModeData.SetWidgetToFocus ( this->TakeWidget () );
//		InputModeData.SetLockMouseToViewportBehavior ( EMouseLockMode::DoNotLock );
//
//		PC->SetInputMode ( InputModeData );
//	}
//
//	LevelSelector->onRestartRequestedDel.AddDynamic ( this , &UP_PauseWidget::RestartGame );
//	LevelSelector->onCloseUIRequestedDel.AddDynamic ( this , &UP_PauseWidget::ResumeGame );
//	LevelSelector->onQuitGameRequestedDel.AddDynamic ( this , &UP_PauseWidget::QuitGame );
//}

void UP_PauseWidget::NativeOnInitialized ()
{
	Super::NativeOnInitialized ();

	if (LevelSelector)
	{
		LevelSelector->onRestartRequestedDel.AddDynamic ( this , &UP_PauseWidget::RestartGame );
		LevelSelector->onCloseUIRequestedDel.AddDynamic ( this , &UP_PauseWidget::ResumeGame );
		LevelSelector->onQuitGameRequestedDel.AddDynamic ( this , &UP_PauseWidget::QuitGame );
	}
}

void UP_PauseWidget::NativeDestruct ()
{
	Super::NativeDestruct ();

	//if (APlayerController* PC = GetOwningPlayer ())
	//{
	//	PC->SetShowMouseCursor ( false );

	//	FInputModeGameOnly InputModeData;
	//	PC->SetInputMode ( InputModeData );
	//}
}

void UP_PauseWidget::RestartGame ()
{
	onRestartRequestedDel.Broadcast ();
}

void UP_PauseWidget::ResumeGame ()
{
	onCloseUIRequested.Broadcast ( this );
}
void UP_PauseWidget::QuitGame ()
{
	onQuitGameRequestedDel.Broadcast ();
}