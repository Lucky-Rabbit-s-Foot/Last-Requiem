#include "PJB/LevelSelector/P_PauseLevelSelector.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UP_PauseLevelSelector::NativeConstruct ()
{
	Super::NativeConstruct ();
	
	if (Btn_Tutorial)
	{
		Btn_Tutorial->OnClicked.RemoveAll(this);
		Btn_Tutorial->OnClicked.AddDynamic ( this , &UP_PauseLevelSelector::OnTutorialButtonClicked );
	}
}

void UP_PauseLevelSelector::OnButton1Clicked ()
{
	onRestartRequestedDel.Broadcast ();
}

void UP_PauseLevelSelector::OnButton2Clicked ()
{
	onCloseUIRequestedDel.Broadcast ();
}

void UP_PauseLevelSelector::OnButton3Clicked ()
{
	onQuitGameRequestedDel.Broadcast ();
}

void UP_PauseLevelSelector::OnTutorialButtonClicked ()
{
	onTutorialRequestedDel.Broadcast ();
}
