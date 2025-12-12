#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DetourCrowdAIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GameplayTagContainer.h"

#include "P_AIControllerEnemyBase.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Damage;
class UBehaviorTree;

UCLASS()
class LASTREQUIEM_API AP_AIControllerEnemyBase : public ADetourCrowdAIController
{
	GENERATED_BODY()
	
public:
	AP_AIControllerEnemyBase ();

	void SetCachedFortressByGameState ();
	AActor* GetCachedFortress () { return CachedFortress.Get (); }

protected:
	virtual void OnPossess ( APawn* InPawn ) override;

	void InitPerceptionComponent ();

private:
	void InitSightConfig ();
	void InitDamageConfig ();

protected:
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|AI" )
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Cached Fortress" )
	TWeakObjectPtr<AActor> CachedFortress;

	UPROPERTY( EditAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception")
	float SightRadius = 1500.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	float LoseSightRadius = 2000.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	float ViewAngleDegree = 90.0f;
};
