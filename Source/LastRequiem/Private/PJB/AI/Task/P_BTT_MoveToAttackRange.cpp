#include "PJB/AI/Task/P_BTT_MoveToAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PJB/Enemy/P_EnemyBase.h"

UP_BTT_MoveToAttackRange::UP_BTT_MoveToAttackRange ()
{
	NodeName = TEXT ( "Move To Attack Range" );
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UP_BTT_MoveToAttackRange::ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory )
{
	AAIController* AIC = OwnerComp.GetAIOwner ();
	AP_EnemyBase* MyPawn = AIC ? Cast<AP_EnemyBase> ( AIC->GetPawn () ) : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent ();
	if (!AIC || !MyPawn || !BB) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor> ( BB->GetValueAsObject ( TEXT ( "TargetActor" ) ) );
	if (!Target) return EBTNodeResult::Failed;

	AIC->SetFocus ( Target );
	MyPawn->GetCharacterMovement ()->bOrientRotationToMovement = false;
	MyPawn->bUseControllerRotationYaw = true;

	FVector MyLoc = MyPawn->GetActorLocation ();
	FVector TargetLoc = Target->GetActorLocation ();

	UPrimitiveComponent* TargetRoot = Cast<UPrimitiveComponent> ( Target->GetRootComponent () );
	if (TargetRoot)
	{
		FVector ClosestPoint;
		if (TargetRoot->GetClosestPointOnCollision ( MyLoc , ClosestPoint ) > 0.0f)
		{
			TargetLoc = ClosestPoint;
		}
	}

	float AttackRange = MyPawn->GetAttackRange ();
	FVector DirectionToMe = (MyLoc - TargetLoc).GetSafeNormal ();
	
	if (DirectionToMe.IsZero ())
	{
		DirectionToMe = FMath::VRand ();
		DirectionToMe.Z = 0.0f;
	}

	FVector IdealLoc = TargetLoc + DirectionToMe * (AttackRange * RangeRatio);
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent ( GetWorld () );
	FNavLocation OutNavLoc;
	if(NavSys && NavSys->ProjectPointToNavigation(IdealLoc, OutNavLoc, FVector(200.0f, 200.0f, 200.0f)))
	{
		IdealLoc = OutNavLoc.Location;
	}

	EPathFollowingRequestResult::Type MoveResult = AIC->MoveToLocation ( IdealLoc , 50.0f );
	if(MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		return EBTNodeResult::Succeeded;
	}
	else if(MoveResult == EPathFollowingRequestResult::Failed)
	{
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::InProgress;
}

void UP_BTT_MoveToAttackRange::TickTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds )
{
	AAIController* AIC = OwnerComp.GetAIOwner ();
	if (!AIC)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (AIC->GetMoveStatus () != EPathFollowingStatus::Moving)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UP_BTT_MoveToAttackRange::OnTaskFinished ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , EBTNodeResult::Type TaskResult )
{
	Super::OnTaskFinished ( OwnerComp , NodeMemory , TaskResult );
	AAIController* AIC = OwnerComp.GetAIOwner ();
	AP_EnemyBase* MyPawn = AIC ? Cast<AP_EnemyBase> ( AIC->GetPawn () ) : nullptr;
	if (AIC && MyPawn)
	{
		AIC->ClearFocus (EAIFocusPriority::Gameplay);
		MyPawn->GetCharacterMovement ()->bOrientRotationToMovement = true;
		MyPawn->bUseControllerRotationYaw = false;
	}
}
