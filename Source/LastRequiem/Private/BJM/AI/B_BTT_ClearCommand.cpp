// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/AI/B_BTT_ClearCommand.h"
#include "BJM/Unit/B_UnitMentalTypes.h"
#include "BehaviorTree/BlackboardComponent.h"

UB_BTT_ClearCommand::UB_BTT_ClearCommand ()
{
	NodeName = TEXT ( "Clear Command" );
}

EBTNodeResult::Type UB_BTT_ClearCommand::ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory )
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent ();
	if (Blackboard)
	{

		Blackboard->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::None );

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
