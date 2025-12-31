#include "PJB/Pause/P_PauseWidget.h"

#include "Components/Button.h"
#include "PJB/Pause/P_TutorialAlbum.h"
#include "PJB/LevelSelector/P_PauseLevelSelector.h"
#include "PJB/Copyright/P_CopyrightListWidget.h"

UP_PauseWidget::UP_PauseWidget ()
{
	UILayer = EUILayer::POPUP;
	zOrder = 10;
}

void UP_PauseWidget::NativeOnInitialized ()
{
	Super::NativeOnInitialized ();

	if (LevelSelector)
	{
		LevelSelector->onRestartRequestedDel.AddDynamic ( this , &UP_PauseWidget::RestartGame );
		LevelSelector->onCloseUIRequestedDel.AddDynamic ( this , &UP_PauseWidget::ResumeGame );
		LevelSelector->onQuitGameRequestedDel.AddDynamic ( this , &UP_PauseWidget::QuitGame );
		LevelSelector->onTutorialRequestedDel.AddDynamic ( this , &UP_PauseWidget::OpenTutorialAlbum );
	}

	CopyrightList->InitCopyrightList ( DA );
	if(Btn_CopyrightIcon)
	{
		Btn_CopyrightIcon->OnHovered.AddDynamic(this, &UP_PauseWidget::OnHoverCopyright);
		Btn_CopyrightIcon->OnUnhovered.AddDynamic(this, &UP_PauseWidget::OnUnhoverCopyright);
	}
}

void UP_PauseWidget::NativeDestruct ()
{
	Super::NativeDestruct ();
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

void UP_PauseWidget::OpenTutorialAlbum ()
{
	onTutorialRequestedDel.Broadcast ();
}

void UP_PauseWidget::OnHoverCopyright()
{
	if (!CopyrightList) return;
	CopyrightList->SetVisibility(ESlateVisibility::Visible);
}

void UP_PauseWidget::OnUnhoverCopyright()
{
	if (!CopyrightList) return;
	CopyrightList->SetVisibility(ESlateVisibility::Hidden);
}
