#pragma once

#include "CoreMinimal.h"
#include "PJB/LevelSelector/P_LevelSelector.h"

#include "P_GameOverLevelSelector.generated.h"

UCLASS()
class LASTREQUIEM_API UP_GameOverLevelSelector : public UP_LevelSelector
{
	GENERATED_BODY()

public:
	virtual void OnButton3Clicked () override;
};
