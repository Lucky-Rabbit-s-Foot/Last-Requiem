#pragma once

#include "CoreMinimal.h"
#include "KHS/UI/K_BaseUIWidget.h"

#include "P_PauseWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnRestartRequestedPause );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnQuitGameRequestedPause );

UCLASS()
class LASTREQUIEM_API UP_PauseWidget : public UK_BaseUIWidget
{
	GENERATED_BODY()
	
	UP_PauseWidget ();

	bool IsOpenTest () { return bIsOpen; }
protected:

	//(251229) B : 일시정지 메뉴 설정을 위해 삽입
	virtual void NativeOnInitialized () override;
	//virtual void NativeConstruct () override;
	virtual void NativeDestruct () override;

private:
	UFUNCTION ()
	void RestartGame ();
	UFUNCTION ()
	void ResumeGame ();
	UFUNCTION ()
	void QuitGame ();

public:
	UPROPERTY ( meta = (BindWidget) )
	class UP_TutorialAlbum* TutorialAlbum;

	UPROPERTY ( meta = (BindWidget) )
	class UP_PauseLevelSelector* LevelSelector;

public:
	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnRestartRequestedPause onRestartRequestedDel;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnQuitGameRequestedPause onQuitGameRequestedDel;
};
