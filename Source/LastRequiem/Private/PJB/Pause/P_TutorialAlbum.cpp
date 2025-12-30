#include "PJB/Pause/P_TutorialAlbum.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "PJB/Data/P_TutorialImageDataAsset.h"

void UP_TutorialAlbum::NativeConstruct ()
{
	Super::NativeConstruct();

	if (Btn_Prev)
	{
		Btn_Prev->OnClicked.RemoveAll ( this );
		Btn_Prev->OnClicked.AddDynamic ( this , &UP_TutorialAlbum::OnPrevClicked );
	}

	if (Btn_Next)
	{
		Btn_Next->OnClicked.RemoveAll ( this );
		Btn_Next->OnClicked.AddDynamic ( this , &UP_TutorialAlbum::OnNextClicked );
	}
	CurrentIndex = 0;
	UpdateTutorialView ();
}

void UP_TutorialAlbum::UpdateTutorialView ()
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

void UP_TutorialAlbum::OnPrevClicked ()
{
	if (CurrentIndex > 0)
	{
		CurrentIndex--;
		UpdateTutorialView ();
	}
}

void UP_TutorialAlbum::OnNextClicked ()
{
	if (CurrentIndex < DA->Images.Num () - 1)
	{
		CurrentIndex++;
		UpdateTutorialView ();
	}
}
