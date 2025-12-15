#include "PJB/AI/Task/P_BTT_MoveToAttackRange.h"

#include "AIController.h"
#include "PJB/Enemy/P_EnemyBase.h"

UP_BTT_MoveToAttackRange::UP_BTT_MoveToAttackRange ()
{
	NodeName = TEXT ( "Move To Attack Range" );
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UP_BTT_MoveToAttackRange::ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory )
{
	AAIController* AIC = OwnerComp.GetAIOwner ();
	AP_EnemyBase* Enemy = AIC ? Cast<AP_EnemyBase> ( AIC->GetPawn () ) : nullptr;
	if (Enemy)
	{
		float MyAttackRange = Enemy->GetAttackRange ();
		float Buffer = 20.0f;
		AcceptableRadius = FMath::Max ( MyAttackRange - Buffer , 50.0f );
	}
	return Super::ExecuteTask ( OwnerComp , NodeMemory );
}
