#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "P_AIControllerEnemyBase.generated.h"

class UBehaviorTree;

UCLASS()
class LASTREQUIEM_API AP_AIControllerEnemyBase : public AAIController
{
	GENERATED_BODY()
	
public:
	AP_AIControllerEnemyBase ();

protected:
	virtual void OnPossess ( APawn* InPawn ) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
};
