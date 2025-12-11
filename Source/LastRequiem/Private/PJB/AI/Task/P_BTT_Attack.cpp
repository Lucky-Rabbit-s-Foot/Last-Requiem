#include "PJB/AI/Task/P_BTT_Attack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UP_BTT_Attack::UP_BTT_Attack ()
{
	NodeName = TEXT ( "Attack" );
}

EBTNodeResult::Type UP_BTT_Attack::ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory )
{
	AAIController* AIC = OwnerComp.GetAIOwner ();
	APawn* Attacker = AIC ? AIC->GetPawn () : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent ();

	if (!Attacker || !BB) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor> ( BB->GetValueAsObject ( TargetActorKey.SelectedKeyName ) );

	if (Target)
	{
		UE_LOG(LogTemp, Log, TEXT ( "%s attacks %s for %f damage." ) ,
			*Attacker->GetName () ,
			*Target->GetName () ,
			DamageAmount
		);
		UGameplayStatics::ApplyDamage (
			Target ,
			DamageAmount ,
			AIC ,           
			Attacker ,      
			UDamageType::StaticClass ()
		);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
