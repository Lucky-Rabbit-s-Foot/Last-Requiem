#pragma once

#include "CoreMinimal.h"
#include "KHS/UI/K_BaseUIWidget.h"

#include "P_LevelSelector.generated.h"

class UButton;

UCLASS()
class LASTREQUIEM_API UP_LevelSelector : public UK_BaseUIWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct () override;

public:
	UPROPERTY ( meta = (BindWidget) )
	UButton* Button1;

	UPROPERTY ( meta = (BindWidget) )
	UButton* Button2;

	UPROPERTY ( meta = (BindWidget) )
	UButton* Button3;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Level" )
	FName Level1Name = "Map_Level01";

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Level" )
	FName Level2Name = "Map_Level02";

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Level" )
	FName Level3Name = "Map_Level03";

protected:
	UFUNCTION ()
	void OnButton1Clicked ();

	UFUNCTION ()
	void OnButton2Clicked ();

	UFUNCTION ()
	void OnButton3Clicked ();

	UFUNCTION ()
	bool IsQuitGame ( FName LevelName );

	void OpenSelectedLevel ( FName LevelName );
};
