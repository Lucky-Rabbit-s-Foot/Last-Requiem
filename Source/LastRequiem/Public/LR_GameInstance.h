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
	
	
	//튜토리얼 UI 플래그
	UPROPERTY ( BlueprintReadWrite , Category = "Tutorial" )
	bool bHasShownTutorial = false;
	
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void MarkTutorialAsShown() {bHasShownTutorial = true;}
	
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	bool CanShowTutorial() const {return !bHasShownTutorial;}
	
};
