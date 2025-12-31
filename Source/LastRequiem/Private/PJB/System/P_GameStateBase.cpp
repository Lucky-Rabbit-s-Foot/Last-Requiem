#include "PJB/System/P_GameStateBase.h"

#include "PJB/Data/P_ScoringDataAsset.h"

AP_GameStateBase::AP_GameStateBase ()
{
	GameResultData.ResetResult ();
}

void AP_GameStateBase::BeginPlay ()
{
	Super::BeginPlay ();

	GetWorldTimerManager ().SetTimer ( ScoreTimerHandle , this , &AP_GameStateBase::AddTimeScore , 1.0f , true );
}

void AP_GameStateBase::CountEnemyKill ( int32 InEnemyID )
{
	switch (InEnemyID)
	{
	case 1:
		GameResultData.Melee++;
		AddScoreForInfo ( ScoreModifier ? ScoreModifier->Melee : 0 );
		break;
	case 2:
		GameResultData.EnMelee++;
		AddScoreForInfo ( ScoreModifier ? ScoreModifier->EnMelee : 0 );
		break;
	case 3:
		GameResultData.Range++;
		AddScoreForInfo ( ScoreModifier ? ScoreModifier->Range : 0 );
		break;
	case 4:
		GameResultData.EnRange++;
		AddScoreForInfo ( ScoreModifier ? ScoreModifier->EnRange : 0 );
		break;
	default:
		break;
	}
}

void AP_GameStateBase::CountRecoveryHealth ()
{
	GameResultData.Heal++;
	AddScoreForInfo ( ScoreModifier ? ScoreModifier->Heal : 0 );
}

void AP_GameStateBase::CountRecoverySanity ()
{
	GameResultData.Sanity++;
	AddScoreForInfo ( ScoreModifier ? ScoreModifier->Sanity : 0 );
}

void AP_GameStateBase::CountDestructEnemyObstacle ()
{
	GameResultData.Dest++;
	AddScoreForInfo ( ScoreModifier ? ScoreModifier->Dest : 0 );
}

void AP_GameStateBase::AddScoreForInfo ( int32 InScore )
{
	TotalScore += InScore;
	OnAddScore.Broadcast ( TotalScore );
}

void AP_GameStateBase::AddTimeScore ()
{
	if (CurrentPhase == EGamePhase::GameOver) return;
	AddScoreForInfo ( ScoreModifier ? ScoreModifier->PlayTime : 0 );
}

void AP_GameStateBase::StopTimeScore ()
{
	GetWorldTimerManager ().ClearTimer ( ScoreTimerHandle );
}

void AP_GameStateBase::SetPlayTime ()
{
	GameResultData.PlayTime = FMath::RoundToInt ( RealGameTimeSeconds );
}
