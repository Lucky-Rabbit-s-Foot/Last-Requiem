// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "B_UnitMentalTypes.h"
#include "B_UnitAIController.generated.h"


class UAISenseConfig_Sight;
class UBehaviorTreeComponent;
class UBlackboardComponent;

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API AB_UnitAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AB_UnitAIController();

	virtual void Tick(float DeltaTime) override;

protected:

	// BT 실행 함수

	virtual void OnPossess(APawn* InPawn) override;

public:

	// 감각 시스템
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;

	// 시야
	class UAISenseConfig_Sight* SightConfig;

	// 비헤이비어 트리 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComponent;

	// 블랙보드
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BTAsset;


	// 적 감지 호출 이벤트
	UFUNCTION()
	void OnTargetDetected(AActor* InActor, FAIStimulus InStimulus);


	void SetCommandState ( EUnitCommandType NewCommand , FVector TargetLocation );

};
