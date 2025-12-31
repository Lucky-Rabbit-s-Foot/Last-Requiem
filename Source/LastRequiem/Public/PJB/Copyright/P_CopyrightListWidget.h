#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "P_CopyrightListWidget.generated.h"

UCLASS()
class LASTREQUIEM_API UP_CopyrightListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct () override;

	void InitCopyrightList ( class UP_CopyrightData* DA );

protected:
	UPROPERTY ( meta = (BindWidget) )
	class UScrollBox* CopyrightScrollBox;

	UPROPERTY ( EditDefaultsOnly , Category = "UI" )
	TSubclassOf<class UP_CopyrightItemWidget> ItemWidgetClass;

private:
	bool IsInitialized = false;
};
