#pragma once

#include "CoreMinimal.h"
#include "PJB/LevelSelector/P_LevelSelector.h"
#include "P_PauseLevelSelector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnRestartRequestedSelect );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnQuitGameRequestedSelect );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnCloseUIRequestedSelect );

UCLASS()
class LASTREQUIEM_API UP_PauseLevelSelector : public UP_LevelSelector
{
	GENERATED_BODY()
	
public:
	virtual void OnButton1Clicked () override;
	virtual void OnButton2Clicked () override;
	virtual void OnButton3Clicked () override;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnRestartRequestedSelect onRestartRequestedDel;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnQuitGameRequestedSelect onQuitGameRequestedDel;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnCloseUIRequestedSelect onCloseUIRequestedDel;
};
