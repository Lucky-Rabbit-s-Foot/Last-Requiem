#include "PJB/LevelSelector/P_LevelSelector.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UP_LevelSelector::NativeConstruct ()
{
	Super::NativeConstruct ();
	if (Button1)
	{
		Button1->OnClicked.RemoveDynamic ( this , &UP_LevelSelector::OnButton1Clicked );
		Button1->OnClicked.AddDynamic ( this , &UP_LevelSelector::OnButton1Clicked );
	}

	if (Button2)
	{
		Button2->OnClicked.RemoveDynamic ( this , &UP_LevelSelector::OnButton2Clicked );
		Button2->OnClicked.AddDynamic ( this , &UP_LevelSelector::OnButton2Clicked );
	}

	if (Button3)
	{
		Button3->OnClicked.RemoveDynamic ( this , &UP_LevelSelector::OnButton3Clicked );
		Button3->OnClicked.AddDynamic ( this , &UP_LevelSelector::OnButton3Clicked );
	}
}

void UP_LevelSelector::OnButton1Clicked ()
{
	if (IsQuitGame ( Level1Name ) || Level1Name.IsNone ()) return;
	OpenSelectedLevel ( Level1Name );
}

void UP_LevelSelector::OnButton2Clicked ()
{
	if (IsQuitGame ( Level2Name ) || Level2Name.IsNone ()) return;
	OpenSelectedLevel ( Level2Name );
}

void UP_LevelSelector::OnButton3Clicked ()
{
	if (IsQuitGame ( Level3Name ) || Level3Name.IsNone ()) return;
	UGameplayStatics::OpenLevel ( this , Level3Name );
}

bool UP_LevelSelector::IsQuitGame ( FName LevelName )
{
	if (LevelName.IsEqual ( FName ( "Quit" ) , ENameCase::IgnoreCase ))
	{
		UKismetSystemLibrary::QuitGame ( GetWorld () , GetWorld ()->GetFirstPlayerController () , EQuitPreference::Quit , true );
		return true;
	}
	return false;
}

void UP_LevelSelector::OpenSelectedLevel ( FName LevelName )
{
	UGameplayStatics::OpenLevel ( this , LevelName );
}
