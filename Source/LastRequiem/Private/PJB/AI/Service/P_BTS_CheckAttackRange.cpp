#include "PJB/AI/Service/P_BTS_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PJB/Enemy/P_EnemyBase.h"

UP_BTS_CheckAttackRange::UP_BTS_CheckAttackRange ()
{
	NodeName = TEXT ( "Check Attack Range" );
	Interval = 0.1f;
	RandomDeviation = 0.02f;
}

void UP_BTS_CheckAttackRange::TickNode ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds )
{
	Super::TickNode ( OwnerComp , NodeMemory , DeltaSeconds );

	APawn* OwnedPawn = OwnerComp.GetAIOwner () ? OwnerComp.GetAIOwner ()->GetPawn () : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent ();
	if (!OwnedPawn || !BB) return;

	AP_EnemyBase* EnemyPawn = Cast<AP_EnemyBase> ( OwnedPawn );
	if (!EnemyPawn) return;

	AActor* Target = Cast<AActor> ( BB->GetValueAsObject ( TargetActorKey.SelectedKeyName ) );

	bool bInRange = false;
	if (Target)
	{
		float AttackRange = EnemyPawn->GetAttackRange ();
		float AttackRangeSq = AttackRange * AttackRange;
		
		float DistSq = FLT_MAX;
		FVector MyLocation = OwnedPawn->GetActorLocation ();

		UPrimitiveComponent* TargetRoot = Cast<UPrimitiveComponent> ( Target->GetRootComponent () );
		if (TargetRoot)
		{
			FVector ClosestPoint;
			float DistToTarget = 0.0f;
			if(TargetRoot->GetClosestPointOnCollision(MyLocation, ClosestPoint) > 0.0f )
			{
				DistSq = FVector::DistSquared ( MyLocation , ClosestPoint );
			}
			else
			{
				DistSq = OwnedPawn->GetSquaredDistanceTo ( Target );
			}
		}
		else
		{
			DistSq = OwnedPawn->GetSquaredDistanceTo ( Target );
		}
		float BufferSq = 5.0f * 5.0f;
		bInRange = (DistSq <= (AttackRangeSq + BufferSq));
	}

	if (bInRange)
	{
		BB->SetValueAsBool ( CanAttackKey.SelectedKeyName , bInRange );
	}
	else
	{
		BB->ClearValue ( CanAttackKey.SelectedKeyName );
	}
}
