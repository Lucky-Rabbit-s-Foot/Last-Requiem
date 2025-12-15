#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "P_EnemyAttackNotify.generated.h"

UCLASS()
class LASTREQUIEM_API UP_EnemyAttackNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify ( USkeletalMeshComponent* MeshComp , UAnimSequenceBase* Animation , const FAnimNotifyEventReference& EventReference ) override;
};
