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
	
	if (Btn_Quit)
	{
		Btn_Quit->OnClicked.RemoveAll ( this );
		Btn_Quit->OnClicked.AddDynamic ( this , &UP_TutorialAlbum::OnQuitClicked );
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

void UP_TutorialAlbum::OnQuitClicked()
{
	onCloseUIRequested.Broadcast(this);
	//한번 보고나면 닫기버튼 가시성 해제
	Btn_Quit->SetVisibility(ESlateVisibility::Hidden);
}
