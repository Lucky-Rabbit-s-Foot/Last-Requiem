#include "PJB/AI/Task/P_BTT_MoveToAttackRange.h"
	
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PJB/Enemy/P_EnemyBase.h"

UP_BTT_MoveToAttackRange::UP_BTT_MoveToAttackRange ()
{
	NodeName = TEXT ( "Move To Attack Range" );
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UP_BTT_MoveToAttackRange::ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory )
{
	AAIController* AIC = OwnerComp.GetAIOwner ();
	AP_EnemyBase* MyPawn = AIC ? Cast<AP_EnemyBase> ( AIC->GetPawn () ) : nullptr;

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent ())
	{
		AActor* TargetActor = Cast<AActor> ( BB->GetValueAsObject ( BlackboardKey.SelectedKeyName ) ); // BlackboardKey는 부모 클래스 변수
		if (!TargetActor)
		{
			return EBTNodeResult::Failed;
		}
	}

	if (MyPawn)
	{
		float AttackRange = MyPawn->GetAttackRange();
		float Buffer = 20.0f;

		float NewRadius = FMath::Max ( AttackRange - Buffer , 50 );
		AcceptableRadius = FValueOrBBKey_Float ( NewRadius );
	}

	return Super::ExecuteTask ( OwnerComp , NodeMemory );
}

EBTNodeResult::Type UP_BTT_MoveToAttackRange::AbortTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory )
{
	AAIController* AIController = OwnerComp.GetAIOwner ();
	if (AIController)
	{
		AIController->StopMovement ();
	}
	return EBTNodeResult::Aborted;
}