#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "P_DetectedObstacleWidget.generated.h"

UCLASS()
class LASTREQUIEM_API UP_DetectedObstacleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void PlayShowAnimation ();
	void PlayHideAnimation ();

protected:
	virtual void NativeConstruct () override;

protected:
	UPROPERTY ( Transient , meta = (BindWidgetAnim) )
	class UWidgetAnimation* AppearAnim;
};
