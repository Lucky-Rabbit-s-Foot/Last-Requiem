#include "PJB/ScoreBoard/P_ScoreBoard.h"

#include "Components/TextBlock.h"
#include "LR_GameInstance.h"
#include "PJB/ScoreBoard/P_ScoreBlock.h"
#include "PJB/Data/P_ScoringDataAsset.h"
#include "PJB/Data/P_GameResultData.h"

void UP_ScoreBoard::NativeConstruct ()
{
	Super::NativeConstruct ();

	ULR_GameInstance* GI = Cast<ULR_GameInstance> ( GetGameInstance () );
	UE_LOG ( LogTemp , Warning , TEXT ( "UP_ScoreBoard::NativeConstruct - GI: %s" ) , GI ? *GI->GetName () : TEXT ( "nullptr" ) );
	if (!GI) return;
	
	UpdateAllScores ( GI->GameResultCount , GI->GameResultScore );

	GI->GameResultCount.LogResult ();
	GI->GameResultScore.LogResult ();
}

void UP_ScoreBoard::UpdateAllScores ( FP_GameResultData& Count , FP_GameResultData& Modifier )
{
	SetScore ( PlayTime , TEXT ( "플레이 시간" ) , Count.PlayTime , Modifier.PlayTime );
	SetScore ( HealthRecovery , TEXT ( "체력 회복" ) , Count.Heal , Modifier.Heal );
	SetScore ( SanityRecovery , TEXT ( "정신력 회복" ) , Count.Sanity , Modifier.Sanity );
	SetScore ( DestroyObject , TEXT ( "구조물 파괴" ) , Count.Dest , Modifier.Dest );

	SetScore ( MeleeCount , TEXT ( "근거리 처치" ) , Count.Melee , Modifier.Melee );
	SetScore ( EnhancedMeleeCount , TEXT ( "강화 근거리 처치" ) , Count.EnMelee , Modifier.EnMelee );
	SetScore ( RangeCount , TEXT ( "원거리 처치" ) , Count.Range , Modifier.Range );
	SetScore ( EnhancedRangeCount , TEXT ( "강화 원거리 처치" ) , Count.EnRange , Modifier.EnRange );

	if (TotalScore) TotalScore->SetText ( FText::AsNumber ( Modifier.TotalScore ) );
}

void UP_ScoreBoard::SetScore ( UP_ScoreBlock* Title , FString TitleText , int32 Count , int32 Modifier )
{
	if (!Title) return;
	Title->SetBlockData ( 
		FText::FromString ( TitleText ) ,
		Count ,
		Count * Modifier
	);
}