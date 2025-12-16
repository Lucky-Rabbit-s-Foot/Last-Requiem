// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "B_OnHitNotify.generated.h"

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UB_OnHitNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UB_OnHitNotify ();

	virtual void Notify ( USkeletalMeshComponent* MeshComp , UAnimSequenceBase* Animation , const FAnimNotifyEventReference& EventReference ) override;


};
