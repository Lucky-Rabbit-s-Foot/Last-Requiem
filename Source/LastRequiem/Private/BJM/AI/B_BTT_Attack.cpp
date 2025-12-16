// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/AI/B_BTT_Attack.h"
#include "BJM/Unit/B_UnitAIController.h"
#include "BJM/Unit/B_UnitBase.h"
#include "BehaviorTree/BlackboardComponent.h"


UB_BTT_Attack::UB_BTT_Attack ()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UB_BTT_Attack::ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory )
{
	// AI 컨트롤러 
	AB_UnitAIController* AIController = Cast<AB_UnitAIController>(OwnerComp.GetAIOwner());
	if (AIController == nullptr) 
	{
		return EBTNodeResult::Failed;
	}

	// 명령을 받을 유닛
	AB_UnitBase* MyUnit = Cast<AB_UnitBase>(AIController->GetPawn());
	if (MyUnit == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// TargetEnemy 가져오기
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard)
	{
		UObject* TargetObject = Blackboard->GetValueAsObject(TEXT("TargetEnemy"));
		
		AActor* TargetActor = Cast<AActor>(TargetObject);

		if (TargetActor)
		{
			MyUnit->UnitAttack(TargetActor);

			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Succeeded;
}
