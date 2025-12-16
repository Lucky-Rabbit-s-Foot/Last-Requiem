#include "PJB/AI/Task/P_BTT_Attack.h"

#include "LastRequiem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PJB/Component/P_CombatComponent.h"
#include "PJB/Enemy/P_EnemyBase.h"

UP_BTT_Attack::UP_BTT_Attack ()
{
	NodeName = TEXT ( "Attack" );

	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UP_BTT_Attack::ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory )
{
	AAIController* AIC = OwnerComp.GetAIOwner ();
	AP_EnemyBase* Attacker = AIC ? Cast<AP_EnemyBase> ( AIC->GetPawn () ) : nullptr;

	if (!Attacker) return EBTNodeResult::Failed;

	UAnimMontage* AttackMontage = Attacker->GetAttackMontage ();
	if (AttackMontage)
	{
		Attacker->PlayAnimMontage ( AttackMontage );
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UP_BTT_Attack::TickTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds )
{
	Super::TickTask ( OwnerComp , NodeMemory , DeltaSeconds );

	AAIController* AIC = OwnerComp.GetAIOwner ();
	AP_EnemyBase* Attacker = AIC ? Cast<AP_EnemyBase> ( AIC->GetPawn () ) : nullptr;

	if (!Attacker)
	{
		FinishLatentTask ( OwnerComp , EBTNodeResult::Failed );
		return;
	}

	UAnimMontage* AttackMontage = Attacker->GetAttackMontage ();
	UAnimInstance* AnimInstance = Attacker->GetMesh ()->GetAnimInstance ();

	bool bIsPlayingMontage = AnimInstance && AnimInstance->IsAnyMontagePlaying ();
	if(!bIsPlayingMontage)
	{
		FinishLatentTask ( OwnerComp , EBTNodeResult::Succeeded );
	}
}
