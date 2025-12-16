#include "PJB/AI/Service/P_BTS_CheckCombatState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PJB/Enemy/P_EnemyBase.h"

UP_BTS_CheckCombatState::UP_BTS_CheckCombatState ()
{
	NodeName = TEXT ( "Check Combat State" );
	Interval = 0.3f;
}

void UP_BTS_CheckCombatState::TickNode ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds )
{
	Super::TickNode ( OwnerComp , NodeMemory , DeltaSeconds );

	APawn* OwnedPawn = OwnerComp.GetAIOwner () ? OwnerComp.GetAIOwner ()->GetPawn () : nullptr;
	AP_EnemyBase* Enemy = Cast<AP_EnemyBase> ( OwnedPawn );
	if (!Enemy) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent ();
	if (!BB) return;

	AActor* TargetActor = Cast<AActor> ( BB->GetValueAsObject ( TargetActorKey.SelectedKeyName ));
	bool bInCombat = false;
	
	if (TargetActor)
	{
		IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface> ( TargetActor );
		if (TagInterface && TagInterface->HasMatchingGameplayTag ( UnitTag ))
		{
			bInCombat = true;
		}
	}
	Enemy->SetCombatState ( bInCombat );
	if (IsCombatKey.SelectedKeyName != NAME_None)
	{
		BB->SetValueAsBool ( IsCombatKey.SelectedKeyName , bInCombat );
	}

}
