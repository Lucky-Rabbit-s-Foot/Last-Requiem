#include "PJB/LevelSelector/P_PauseLevelSelector.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UP_PauseLevelSelector::OnButton1Clicked ()
{
	onRestartRequestedDel.Broadcast ();
}

void UP_PauseLevelSelector::OnButton2Clicked ()
{
	onCloseUIRequested.Broadcast ( this );
}

void UP_PauseLevelSelector::OnButton3Clicked ()
{
	onQuitGameRequestedDel.Broadcast ();
}
