#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "P_MainMenu_Help.generated.h"

class UButton;
class UImage;
class UP_TutorialImageDataAsset;

UCLASS()
class LASTREQUIEM_API UP_MainMenu_Help : public UUserWidget
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
	UButton* Btn_Quit;

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

	//(K) 닫기 버튼 추가
	UFUNCTION ()
	void OnQuitClicked ();
};
