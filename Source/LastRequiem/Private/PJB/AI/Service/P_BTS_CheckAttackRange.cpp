#include "PJB/AI/Service/P_BTS_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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

	AActor* Target = Cast<AActor> ( BB->GetValueAsObject ( TargetActorKey.SelectedKeyName ) );

	bool bInRange = false;
	if (Target)
	{
		float Distance = OwnedPawn->GetDistanceTo ( Target );

		bInRange = (Distance <= AttackRange);
	}
	BB->SetValueAsBool ( CanAttackKey.SelectedKeyName , bInRange );
}
