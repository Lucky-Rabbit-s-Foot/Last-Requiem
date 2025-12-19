#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GameplayTagContainer.h"
#include "P_BTS_SelectTarget.generated.h"

class AP_AIControllerEnemyBase;

UCLASS()
class LASTREQUIEM_API UP_BTS_SelectTarget : public UBTService
{
	GENERATED_BODY()
	
public:
	UP_BTS_SelectTarget ();

protected:
	virtual void TickNode ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds ) override;
	
	bool HasGameplayTag ( AActor* Actor , FGameplayTag Tag ) const;
private:
	void FindObstacle ( AP_AIControllerEnemyBase* AIC , APawn* OwnedPawn , AActor*& FinalTarget , int32& FinalTypeInt );
	void FindUnits ( AP_AIControllerEnemyBase* AIC , APawn* OwnedPawn , AActor*& FinalTarget , int32& FinalTypeInt );
	void FindFortress ( AP_AIControllerEnemyBase* AIC , AActor*& FinalTarget , int32& FinalTypeInt );

protected:
	UPROPERTY ( EditAnywhere , Category = "Data|AI|Tags" )
	FGameplayTag UnitTag;
	UPROPERTY ( EditAnywhere , Category = "Data|AI|Tags" )
	FGameplayTag ObstacleTag;
	UPROPERTY ( EditAnywhere , Category = "Data|AI|Tags" )
	FGameplayTag FortressTag;

	UPROPERTY ( EditAnywhere , Category = "Data|AI|Settings" )
	float ObstacleCheckRadius = 500.0f;

	UPROPERTY ( EditAnywhere , Category = "Data|Blackboard" )
	FBlackboardKeySelector TargetActorKey;
	UPROPERTY ( EditAnywhere , Category = "Data|Blackboard" )
	FBlackboardKeySelector TagKey;
};
