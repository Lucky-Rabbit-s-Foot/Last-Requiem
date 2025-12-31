#include "PJB/Pause/P_MainMenu_Help.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "PJB/Data/P_TutorialImageDataAsset.h"

void UP_MainMenu_Help::NativeConstruct ()
{
	Super::NativeConstruct ();

	if (Btn_Prev)
	{
		Btn_Prev->OnClicked.RemoveAll ( this );
		Btn_Prev->OnClicked.AddDynamic ( this , &UP_MainMenu_Help::OnPrevClicked );
	}

	if (Btn_Next)
	{
		Btn_Next->OnClicked.RemoveAll ( this );
		Btn_Next->OnClicked.AddDynamic ( this , &UP_MainMenu_Help::OnNextClicked );
	}

	if (Btn_Quit)
	{
		Btn_Quit->OnClicked.RemoveAll ( this );
		Btn_Quit->OnClicked.AddDynamic ( this , &UP_MainMenu_Help::OnQuitClicked );
	}

	CurrentIndex = 0;
	UpdateTutorialView ();

	SetVisibility ( ESlateVisibility::Hidden );
}

void UP_MainMenu_Help::UpdateTutorialView ()
{
	if (!DA || DA->Images.Num () == 0) return;

	if (Img_Tutorial && DA->Images.IsValidIndex ( CurrentIndex ))
	{
		Img_Tutorial->SetBrushFromTexture ( DA->Images[CurrentIndex] );
	}

	if (Btn_Prev)
	{
		Btn_Prev->SetIsEnabled ( CurrentIndex > 0 );
	}

	if (Btn_Next)
	{
		Btn_Next->SetIsEnabled ( CurrentIndex < DA->Images.Num () - 1 );
	}
}


void UP_MainMenu_Help::OnPrevClicked ()
{
	if (CurrentIndex > 0)
	{
		CurrentIndex--;
		UpdateTutorialView ();
	}
}

void UP_MainMenu_Help::OnNextClicked ()
{
	if (CurrentIndex < DA->Images.Num () - 1)
	{
		CurrentIndex++;
		UpdateTutorialView ();
	}
}

void UP_MainMenu_Help::OnQuitClicked ()
{
	SetVisibility ( ESlateVisibility::Hidden );
	CurrentIndex = 0;
	UpdateTutorialView ();
}