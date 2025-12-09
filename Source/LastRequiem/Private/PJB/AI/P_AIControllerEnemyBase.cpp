#include "PJB/AI/P_AIControllerEnemyBase.h"

#include "BehaviorTree/BehaviorTree.h"

AP_AIControllerEnemyBase::AP_AIControllerEnemyBase ()
{
}

void AP_AIControllerEnemyBase::OnPossess ( APawn* InPawn )
{
	Super::OnPossess ( InPawn );

	if (BehaviorTree)
	{
		RunBehaviorTree ( BehaviorTree );
	}
}
