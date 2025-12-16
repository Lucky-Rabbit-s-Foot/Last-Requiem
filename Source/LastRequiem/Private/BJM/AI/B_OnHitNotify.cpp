// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/AI/B_OnHitNotify.h"
#include "BJM/Unit/B_UnitBase.h"
#include "Animation/AnimNotifies/AnimNotify.h"


UB_OnHitNotify::UB_OnHitNotify ()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor::Red;
#endif
}
void UB_OnHitNotify::Notify ( USkeletalMeshComponent* MeshComp , UAnimSequenceBase* Animation , const FAnimNotifyEventReference& EventReference )
{
	Super::Notify ( MeshComp , Animation , EventReference );

	if (MeshComp && MeshComp->GetOwner ())
	{
		AB_UnitBase* Unit = Cast<AB_UnitBase> ( MeshComp->GetOwner () );

		if (Unit && Unit->IsAlive ())
		{
			Unit->OnAttackHit_Unit (); 
		}
	}
}
