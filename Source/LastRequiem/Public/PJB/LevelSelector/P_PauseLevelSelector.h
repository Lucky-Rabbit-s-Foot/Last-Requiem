#pragma once

#include "CoreMinimal.h"
#include "PJB/LevelSelector/P_LevelSelector.h"
#include "P_PauseLevelSelector.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnRestartRequested );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnQuitGameRequested );

UCLASS()
class LASTREQUIEM_API UP_PauseLevelSelector : public UP_LevelSelector
{
	GENERATED_BODY()
	
public:
	virtual void OnButton1Clicked () override;
	virtual void OnButton2Clicked () override;
	virtual void OnButton3Clicked () override;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnRestartRequested onRestartRequestedDel;

	UPROPERTY ( BlueprintAssignable , Category = "LR|Events" )
	FOnQuitGameRequested onQuitGameRequestedDel;
};
