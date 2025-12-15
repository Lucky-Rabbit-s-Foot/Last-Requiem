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
	virtual void TickTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds ) override;

protected:
	UPROPERTY ( EditANywhere , Category = "Data|AI|Blackboard" )
	FBlackboardKeySelector TargetActorKey;
};
