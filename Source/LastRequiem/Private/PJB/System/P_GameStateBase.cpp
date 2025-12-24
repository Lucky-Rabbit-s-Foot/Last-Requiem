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
