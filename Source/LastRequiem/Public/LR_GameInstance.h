#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PJB/Data/P_GameResultData.h"

#include "LR_GameInstance.generated.h"

UCLASS()
class LASTREQUIEM_API ULR_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY ( BlueprintReadWrite , Category = "GameResult" )
	FP_GameResultData GameResultCount;

	UPROPERTY ( BlueprintReadWrite , Category = "GameResult" )
	FP_GameResultData GameResultScore;
};
