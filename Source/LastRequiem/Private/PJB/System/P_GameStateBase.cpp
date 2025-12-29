#include "PJB/System/P_GameStateBase.h"

AP_GameStateBase::AP_GameStateBase ()
{
	GameResultData.ResetResult ();
}

void AP_GameStateBase::CountEnemyKill ( int32 InEnemyID )
{
	switch (InEnemyID)
	{
	case 1:
		GameResultData.Melee++;
		break;
	case 2:
		GameResultData.EnMelee++;
		break;
	case 3:
		GameResultData.Range++;
		break;
	case 4:
		GameResultData.EnRange++;
		break;
	default:
		break;
	}
}

void AP_GameStateBase::CountRecoveryHealth ()
{
	GameResultData.Heal++;
}

void AP_GameStateBase::CountRecoverySanity ()
{
	GameResultData.Sanity++;
}

void AP_GameStateBase::CountDestructEnemyObstacle ()
{
	GameResultData.Dest++;
}

void AP_GameStateBase::SetPlayTime ()
{
	GameResultData.PlayTime = FMath::RoundToInt ( RealGameTimeSeconds );
}
