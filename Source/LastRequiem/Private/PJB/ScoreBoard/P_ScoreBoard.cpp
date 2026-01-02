#include "PJB/ScoreBoard/P_ScoreBoard.h"

#include "Components/TextBlock.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h" 
#include "Components/Image.h"

#include "LR_GameInstance.h"
#include "PJB/ScoreBoard/P_ScoreBlock.h"
#include "PJB/Data/P_ScoringDataAsset.h"
#include "PJB/Data/P_GameResultData.h"
#include "PJB/Data/P_RankDataAsset.h"
#include "PJB/LevelSelector/P_GameOverLevelSelector.h"

void UP_ScoreBoard::NativeConstruct ()
{
	Super::NativeConstruct ();

	LevelSelector->SetIsEnabled ( false );

	ULR_GameInstance* GI = Cast<ULR_GameInstance> ( GetGameInstance () );
	UE_LOG ( LogTemp , Warning , TEXT ( "UP_ScoreBoard::NativeConstruct - GI: %s" ) , GI ? *GI->GetName () : TEXT ( "nullptr" ) );
	if (!GI) return;
	
	UpdateAllScores ( GI->GameResultCount , GI->GameResultScore );

	GI->GameResultCount.LogResult ();
	GI->GameResultScore.LogResult ();

	if (ScoreUpdateAnim)
	{
		PlayAnimation ( ScoreUpdateAnim );

		FWidgetAnimationDynamicEvent AnimFinishedDelegate;
		AnimFinishedDelegate.BindDynamic ( this , &UP_ScoreBoard::OnScoreUpdateAnimFinished );
		BindToAnimationFinished ( ScoreUpdateAnim , AnimFinishedDelegate );
	}
}

void UP_ScoreBoard::NativeDestruct ()
{
	Super::NativeDestruct ();

	if (BGMComponent && BGMComponent->IsPlaying ())
	{
		BGMComponent->Stop ();
	}
}

void UP_ScoreBoard::UpdateAllScores ( FP_GameResultData& Count , FP_GameResultData& Score )
{
	SetScore ( PlayTime , TEXT ( "플레이 시간" ) , Count.PlayTime , Score.PlayTime );
	SetScore ( HealthRecovery , TEXT ( "아군 체력 회복" ) , Count.Heal , Score.Heal );
	SetScore ( SanityRecovery , TEXT ( "아군 정신력 회복" ) , Count.Sanity , Score.Sanity );
	SetScore ( DestroyObject , TEXT ( "봉인 장치 파괴" ) , Count.Dest , Score.Dest );

	SetScore ( MeleeCount , TEXT ( "근거리 처치" ) , Count.Melee , Score.Melee );
	SetScore ( EnhancedMeleeCount , TEXT ( "강화 근거리 처치" ) , Count.EnMelee , Score.EnMelee );
	SetScore ( RangeCount , TEXT ( "원거리 처치" ) , Count.Range , Score.Range );
	SetScore ( EnhancedRangeCount , TEXT ( "강화 원거리 처치" ) , Count.EnRange , Score.EnRange );

	if (TotalScore) TotalScore->SetText ( FText::AsNumber ( Score.TotalScore ) );
	UpdateRankImage ( Score.TotalScore );
}

void UP_ScoreBoard::SetScore ( UP_ScoreBlock* Title , FString TitleText , int32 Count , int32 Score )
{
	if (!Title) return;
	Title->SetBlockData ( 
		FText::FromString ( TitleText ) ,
		Count ,
		Score
	);
}

void UP_ScoreBoard::OnScoreUpdateAnimFinished ()
{
	LevelSelector->SetIsEnabled ( true );

	if (GameOverBGM)
	{
		if (!BGMComponent || !BGMComponent->IsPlaying ())
		{
			BGMComponent = UGameplayStatics::SpawnSound2D ( this , GameOverBGM );
		}
	}
}

void UP_ScoreBoard::UpdateRankImage ( int32 InTotalScore )
{
	if (!RankDataAsset) return;

	for (const FRankData& RankData : RankDataAsset->RankList)
	{
		if (InTotalScore >= RankData.MinScoreRequirement)
		{
			if (RankData.RankImage)
			{
				Image_Score->SetBrushFromTexture ( RankData.RankImage );
			}
			return;
		}
	}
}
