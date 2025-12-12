// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/AI/B_BTT_Stop.h"
#include "AIController.h"

UB_BTT_Stop::UB_BTT_Stop ()
{
	NodeName = TEXT ( "Stop Movement" );
}

EBTNodeResult::Type UB_BTT_Stop::ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory )
{
	AAIController* AIController = OwnerComp.GetAIOwner ();
	if (AIController)
	{
		AIController->StopMovement ();
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
