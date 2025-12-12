#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "P_BTT_Attack.generated.h"

UCLASS()
class LASTREQUIEM_API UP_BTT_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UP_BTT_Attack ();
	
	virtual EBTNodeResult::Type ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory ) override;

protected:
	UPROPERTY ( EditAnywhere , Category = "Data|AI|Attack" )
	float DamageAmount = 10.0f;

	UPROPERTY ( EditANywhere , Category = "Data|AI|Blackboard" )
	FBlackboardKeySelector TargetActorKey;
};
