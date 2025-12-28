#pragma once

#include "CoreMinimal.h"
#include "KHS/UI/K_BaseUIWidget.h"

#include "P_ScoreBoard.generated.h"

class UP_ScoreBlock;
class UTextBlock;
class UP_ScoringDataAsset;
class USoundBase;
class UAudioComponent;

UCLASS()
class LASTREQUIEM_API UP_ScoreBoard : public UK_BaseUIWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct () override;
	virtual void NativeDestruct () override;

	UFUNCTION(BlueprintCallable, Category = "UI|Score" )
	void UpdateAllScores ( FP_GameResultData& Count , FP_GameResultData& Modifier );

	void SetScore ( UP_ScoreBlock* Title, FString TitleText , int32 PlayTimeSec , int32 Modifier);

	UFUNCTION ()
	void OnScoreUpdateAnimFinished ();

protected:
	// Play Score
	UPROPERTY ( meta = (BindWidget) )
	UP_ScoreBlock* PlayTime;
	UPROPERTY ( meta = (BindWidget) )
	UP_ScoreBlock* HealthRecovery;
	UPROPERTY ( meta = (BindWidget) )
	UP_ScoreBlock* SanityRecovery;
	UPROPERTY ( meta = (BindWidget) )
	UP_ScoreBlock* DestroyObject;

	// Kill Score
	UPROPERTY ( meta = (BindWidget) )
	UP_ScoreBlock* MeleeCount;
	UPROPERTY ( meta = (BindWidget) )
	UP_ScoreBlock* EnhancedMeleeCount;
	UPROPERTY ( meta = (BindWidget) )
	UP_ScoreBlock* RangeCount;
	UPROPERTY ( meta = (BindWidget) )
	UP_ScoreBlock* EnhancedRangeCount;

	// Total Score
	UPROPERTY ( meta = (BindWidget) )
	UTextBlock* TotalScore;

	UPROPERTY ( meta = (BindWidget) )
	class UP_GameOverLevelSelector* LevelSelector;

	UPROPERTY( Transient , meta = (BindWidgetAnim) )
	class UWidgetAnimation* ScoreUpdateAnim;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Audio|Score" )
	USoundBase* GameOverBGM;

private:
	UPROPERTY ()
	UAudioComponent* BGMComponent;
};
