#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GameplayTagContainer.h"
#include "P_BTS_CheckCombatState.generated.h"

UCLASS()
class LASTREQUIEM_API UP_BTS_CheckCombatState : public UBTService
{
	GENERATED_BODY()

public:
	UP_BTS_CheckCombatState ();

protected:
	void TickNode ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds ) override;
	
public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "AI|Attack" )
	FBlackboardKeySelector TargetActorKey;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "AI|Attack" )
	FBlackboardKeySelector IsCombatKey;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "AI|Tags" )
	FGameplayTag UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
};
