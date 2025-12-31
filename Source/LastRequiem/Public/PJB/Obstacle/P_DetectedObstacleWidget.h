#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P_DetectedObstacleWidget.generated.h"

UCLASS()
class LASTREQUIEM_API UP_DetectedObstacleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct () override;

public:
	void PlayShowAnimation ();
	void PlayHideAnimation ();

protected:
	UFUNCTION ()
	void OnHideAnimFinished ();

protected:
	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	class UWidgetAnimation* AppearAnim;
};
