#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "P_PhaseInfoWidget.generated.h"

class UTextBlock;

UCLASS()
class LASTREQUIEM_API UP_PhaseInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct () override;

	UFUNCTION ()
	void RefreshWave ();
	UFUNCTION ()
	void RefreshScore ( int32 InScore );

public:
	UPROPERTY ( meta = (BindWidget) )
	UTextBlock* Current_Wave;

	UPROPERTY ( meta = (BindWidget) )
	UTextBlock* Total_Wave;

	UPROPERTY ( meta = (BindWidget) )
	UTextBlock* Score;

};
