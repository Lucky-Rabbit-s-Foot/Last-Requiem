#include "PJB/AnimNotify/P_EnemyAttackNotify.h"

#include "PJB/Enemy/P_EnemyBase.h"

void UP_EnemyAttackNotify::Notify ( USkeletalMeshComponent* MeshComp , UAnimSequenceBase* Animation , const FAnimNotifyEventReference& EventReference )
{
	Super::Notify ( MeshComp , Animation , EventReference );

	if (MeshComp && MeshComp->GetOwner ())
	{
		if (AP_EnemyBase* Enemy = Cast<AP_EnemyBase> ( MeshComp->GetOwner () ))
		{
			Enemy->OnAttack ();
		}
	}
}
