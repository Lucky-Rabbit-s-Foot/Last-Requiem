#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"

#include "P_BTT_MoveToAttackRange.generated.h"

UCLASS()
class LASTREQUIEM_API UP_BTT_MoveToAttackRange : public UBTTask_MoveTo
{
	GENERATED_BODY()
	
public:
	UP_BTT_MoveToAttackRange ();

	virtual EBTNodeResult::Type ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory ) override;
	virtual EBTNodeResult::Type AbortTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory ) override;
};
