#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"

#include "P_BTS_CheckAttackRange.generated.h"

UCLASS()
class LASTREQUIEM_API UP_BTS_CheckAttackRange : public UBTService
{
	GENERATED_BODY()

public:
	UP_BTS_CheckAttackRange ();

protected:
	virtual void TickNode ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds ) override;

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "AI|Attack" )
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "AI|Attack" )
	FBlackboardKeySelector CanAttackKey;
};
