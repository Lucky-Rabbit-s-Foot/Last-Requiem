#include "PJB/AI/Service/P_BTS_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PJB/Enemy/P_EnemyBase.h"
#include "PJB/Obstacle/P_Obstacle.h"

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
	AActor* Target = Cast<AActor> ( BB->GetValueAsObject ( TargetActorKey.SelectedKeyName ) );
	if (!EnemyPawn || !IsValid(Target))
	{
		BB->ClearValue ( CanAttackKey.SelectedKeyName );
		return;
	}

	bool bIsTargetValid = true;
	if (AP_Obstacle* ObstacleTarget = Cast<AP_Obstacle> ( Target ))
	{
		if (ObstacleTarget->IsBroken ())
		{
			bIsTargetValid = false;
		}
	}

	if (!bIsTargetValid)
	{
		BB->ClearValue ( CanAttackKey.SelectedKeyName );
		BB->ClearValue ( TargetActorKey.SelectedKeyName );
		return;
	}

	bool bInRange = false;
	bool bWasAttacking = BB->GetValueAsBool ( CanAttackKey.SelectedKeyName );

	float AttackRange = EnemyPawn->GetAttackRange ();
	float CheckRange = bWasAttacking ? AttackRange + 50.0f : AttackRange;
	float CheckRangeSq = CheckRange * CheckRange;

	float DistSq = FLT_MAX;
	FVector MyLocation = OwnedPawn->GetActorLocation ();

	UPrimitiveComponent* TargetRoot = Cast<UPrimitiveComponent> ( Target->GetRootComponent () );
	if (TargetRoot)
	{
		FVector ClosestPoint;
		if (TargetRoot->GetClosestPointOnCollision ( MyLocation , ClosestPoint ) > 0.0f)
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
	bInRange = (DistSq <= CheckRangeSq);

	if (bInRange != bWasAttacking)
	{
		if (bInRange)
		{
			BB->SetValueAsBool ( CanAttackKey.SelectedKeyName , true );
		}
		else
		{
			BB->ClearValue ( CanAttackKey.SelectedKeyName );
		}
	}
}
