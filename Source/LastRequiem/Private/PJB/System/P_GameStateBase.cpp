#include "PJB/System/P_GameStateBase.h"

AP_GameStateBase::AP_GameStateBase ()
{
	TotalScore = 0;
}

void AP_GameStateBase::AddScore ( int32 Amount )
{
	TotalScore += Amount;
}
