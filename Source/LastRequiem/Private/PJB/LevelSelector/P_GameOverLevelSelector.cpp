#include "PJB/LevelSelector/P_GameOverLevelSelector.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UP_GameOverLevelSelector::OnButton3Clicked ()
{
	UKismetSystemLibrary::QuitGame ( GetWorld () , GetWorld ()->GetFirstPlayerController () , EQuitPreference::Quit , true );
	return;
}
