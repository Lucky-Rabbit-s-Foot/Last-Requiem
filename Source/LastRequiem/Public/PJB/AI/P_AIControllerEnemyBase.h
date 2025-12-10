#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GameplayTagContainer.h"

#include "P_AIControllerEnemyBase.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Damage;
class UBehaviorTree;

UCLASS()
class LASTREQUIEM_API AP_AIControllerEnemyBase : public AAIController
{
	GENERATED_BODY()
	
public:
	AP_AIControllerEnemyBase ();

	void InitGameplayTags ();

	void InitDamageConfig ();

	void InitSightConfig ();

	void SightRangeSetting ();

	void SightDetectionSetting ();

protected:
	virtual void OnPossess ( APawn* InPawn ) override;

	void BindPerceptionUpdate ();

	UFUNCTION()
	void OnTargetPerceived ( AActor* InActor , FAIStimulus Stimulus );

	void UpdateBestTarget ();
	void FindClosestUnitOnDamaging ( APawn* PossedPawn , float& MinDistSq , AActor*& ClosestUnit );
	void FindClosestUnitOnVisible ( APawn* PossedPawn , float& MinDistSq , AActor*& ClosestUnit );
	bool IsUnit ( AActor* TargetActor ) const;

protected:
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	TObjectPtr<UAISense_Damage> DamageConfig;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|AI" )
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY ( EditDefaultsOnly , Category = "Data|AI|Tags" )
	FGameplayTag UnitTag;
	UPROPERTY ( EditDefaultsOnly , Category = "Data|AI|Tags" )
	FGameplayTag ObstacleTag;
	UPROPERTY ( EditDefaultsOnly , Category = "Data|AI|Tags" )
	FGameplayTag FortressTag;
	
	TWeakObjectPtr<AActor> CachedFortress;


	UPROPERTY( EditAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception")
	float SightRadius = 1500.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	float LoseSightRadius = 2000.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	float ViewAngleDegree = 90.0f;
};
