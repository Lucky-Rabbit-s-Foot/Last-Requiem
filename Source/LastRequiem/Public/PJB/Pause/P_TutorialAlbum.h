#pragma once

#include "CoreMinimal.h"
#include "KHS/UI/K_BaseUIWidget.h"

#include "P_TutorialAlbum.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UTexture2D;
class UP_TutorialImageDataAsset;

UCLASS()
class LASTREQUIEM_API UP_TutorialAlbum : public UK_BaseUIWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct () override;

public:
	UPROPERTY ( meta = (BindWidget) )
	UButton* Btn_Prev;

	UPROPERTY ( meta = (BindWidget) )
	UButton* Btn_Next;

	UPROPERTY ( meta = (BindWidget) )
	UImage* Img_Tutorial;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Tutorial Data" )
	TObjectPtr<UP_TutorialImageDataAsset> DA;
private:
	int32 CurrentIndex = 0;

	void UpdateTutorialView ();

	UFUNCTION ()
	void OnPrevClicked ();

	UFUNCTION ()
	void OnNextClicked ();
};
