#pragma once

#include "CoreMinimal.h"
#include "PJB/LevelSelector/P_LevelSelector.h"
#include "P_PauseLevelSelector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnRestartRequestedSelect );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnQuitGameRequestedSelect );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnCloseUIRequestedSelect );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnTutorialRequestedSelect );

UCLASS()
class LASTREQUIEM_API UP_PauseLevelSelector : public UP_LevelSelector
{
	GENERATED_BODY()
	
public:
	void NativeConstruct () override;

	virtual void OnButton1Clicked () override;
	virtual void OnButton2Clicked () override;
	virtual void OnButton3Clicked () override;

	UFUNCTION ()
	virtual void OnTutorialButtonClicked ();

	UPROPERTY ( meta = (BindWidget) )
	UButton* Btn_Tutorial;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnRestartRequestedSelect onRestartRequestedDel;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnQuitGameRequestedSelect onQuitGameRequestedDel;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnCloseUIRequestedSelect onCloseUIRequestedDel;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnTutorialRequestedSelect onTutorialRequestedDel;
};
