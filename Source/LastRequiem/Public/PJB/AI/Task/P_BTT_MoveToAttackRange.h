#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"

#include "P_BTT_MoveToAttackRange.generated.h"

UCLASS()
class LASTREQUIEM_API UP_BTT_MoveToAttackRange : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UP_BTT_MoveToAttackRange ();

	virtual EBTNodeResult::Type ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory ) override;
	virtual void TickTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds ) override;
	virtual void OnTaskFinished ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , EBTNodeResult::Type TaskResult ) override;
protected:
	UPROPERTY ( EditAnywhere , Category = "Data|BlackBoard" )
	struct FBlackboardKeySelector TargetActorKey;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|AI|Attack" )
	float RangeRatio = 0.8f;
};
