// Fill out your copyright notice in the Description page of Project Settings.


#include "B_BTD_IsInAttackRange.h"
#include "BJM/Unit/B_UnitAIController.h"
#include "BJM/Unit/B_UnitBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UB_BTD_IsInAttackRange::UB_BTD_IsInAttackRange ()
{
	NodeName = TEXT ( "Is In Attack Range" );
}

bool UB_BTD_IsInAttackRange::CalculateRawConditionValue ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory ) const
{
	AAIController* AIController = OwnerComp.GetAIOwner ();
	if (!AIController) return false;

	AB_UnitBase* MyPawn = Cast<AB_UnitBase> ( AIController->GetPawn () );
	if (!MyPawn) return false;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent ();
	if (!Blackboard) return false;

	AActor* TargetActor = Cast<AActor> ( Blackboard->GetValueAsObject ( TEXT ( "TargetEnemy" ) ) );

	if (!TargetActor) return false;

	float Distance = FVector::Dist2D ( MyPawn->GetActorLocation () , TargetActor->GetActorLocation () );

	return Distance <= (MyPawn->AttackRange + 100.0f);
}
