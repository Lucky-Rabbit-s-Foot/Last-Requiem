#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"

#include "P_AIControllerEnemyBase.generated.h"

class UBehaviorTree;
class UAISenseConfig_Sight;
class UAISenseConfig_Damage;

UCLASS()
class LASTREQUIEM_API AP_AIControllerEnemyBase : public AAIController
{
	GENERATED_BODY()
	
public:
	AP_AIControllerEnemyBase ();

protected:
	virtual void OnPossess ( APawn* InPawn ) override;

	UFUNCTION()
	void OnTargetPerceived ( AActor* InActor , FAIStimulus Stimulus );

	void UpdateBestTarget ();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|AI|Perception")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|AI|Perception")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|AI|Perception")
	float SightRadius = 1500.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	float LoseSightRadius = 2000.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|AI|Perception" )
	float ViewAngleDegree = 90.0f;

};
