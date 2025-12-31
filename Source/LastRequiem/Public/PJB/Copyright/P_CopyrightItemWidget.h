#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PJB/Data/P_CopyrightData.h"

#include "P_CopyrightItemWidget.generated.h"

UCLASS()
class LASTREQUIEM_API UP_CopyrightItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetCopyrightData ( const FCopyrightInfo& Info );

protected:
	UPROPERTY ( meta = (BindWidget) )
	class UTextBlock* AssetName;

	UPROPERTY ( meta = (BindWidget) )
	class UTextBlock* License;

	UPROPERTY ( meta = (BindWidget) )
	class UTextBlock* Author;
};
