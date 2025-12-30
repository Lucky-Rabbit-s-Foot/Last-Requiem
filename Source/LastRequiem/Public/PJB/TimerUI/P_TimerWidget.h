#pragma once

#include "CoreMinimal.h"
#include "KHS/UI/K_BaseUIWidget.h"

#include "P_TimerWidget.generated.h"

UCLASS()
class LASTREQUIEM_API UP_TimerWidget : public UK_BaseUIWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct () override;
	virtual void NativeTick ( const FGeometry& MyGeometry , float InDeltaTime ) override;

protected:
	UPROPERTY ( meta = ( BindWidget ) )
	class UTextBlock* TimerText;

	UPROPERTY ()
	class AP_GameStateBase* GameStateBase;
};
