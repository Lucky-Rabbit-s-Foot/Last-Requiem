// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/AI/B_AN_UnitFireSound.h"
#include "BJM/Unit/B_UnitBase.h"

void UB_AN_UnitFireSound::Notify ( USkeletalMeshComponent* MeshComp , UAnimSequenceBase* Animation , const FAnimNotifyEventReference& EventReference )
{
	Super::Notify ( MeshComp , Animation , EventReference );

	if (MeshComp && MeshComp->GetOwner ())
	{
		AB_UnitBase* Unit = Cast<AB_UnitBase> ( MeshComp->GetOwner () );
		if (Unit)
		{
			Unit->PlayWeaponFireSound ();
		}
	}
}
