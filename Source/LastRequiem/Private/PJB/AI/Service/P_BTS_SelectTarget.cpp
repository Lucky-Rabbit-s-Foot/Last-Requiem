#include "PJB/AI/Service/P_BTS_SelectTarget.h"

#include "LastRequiem.h"
#include "AIController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/OverlapResult.h"
#include "PJB/System/P_GameStateBase.h"
#include "PJB/Enemy/P_EnemyBase.h"
#include "PJB/AI/P_AIControllerEnemyBase.h"
#include "PJB/Obstacle/P_Obstacle.h"

UP_BTS_SelectTarget::UP_BTS_SelectTarget ()
{
	NodeName = TEXT ( "Select Target" );
	Interval = 0.5f;
}

void UP_BTS_SelectTarget::TickNode ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds )
{
	Super::TickNode ( OwnerComp , NodeMemory , DeltaSeconds );

	AP_AIControllerEnemyBase* AIC = Cast<AP_AIControllerEnemyBase> ( OwnerComp.GetAIOwner () );
	if (!AIC) return;

	APawn* OwnedPawn = AIC->GetPawn ();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent ();
	if (!OwnedPawn || !BB) return;

	AActor* FinalTarget = nullptr;
	int32 FinalTypeInt = 0;

	AActor* CurrentTarget = Cast<AActor> ( BB->GetValueAsObject ( TargetActorKey.SelectedKeyName ) );
	bool bShouldKeepCurrentTarget = false;

	if (IsValid ( CurrentTarget ) && HasGameplayTag ( CurrentTarget , ObstacleTag ))
	{
		if (AP_Obstacle* CurrentObstacle = Cast<AP_Obstacle> ( CurrentTarget ))
		{
			if (!CurrentObstacle->IsBroken ())
			{
				float DistSq = FVector::DistSquared ( OwnedPawn->GetActorLocation () , CurrentObstacle->GetActorLocation () );

				float KeepRadius = ObstacleCheckRadius * 2.0f;
				float KeepRangeSq = KeepRadius * KeepRadius;

				if (DistSq <= KeepRangeSq)
				{
					FinalTarget = CurrentTarget;
					FinalTypeInt = 2;
					bShouldKeepCurrentTarget = true;
				}
			}
		}
	}

	if (!bShouldKeepCurrentTarget)
	{
		FindObstacle ( AIC , OwnedPawn , FinalTarget , FinalTypeInt );
		FindUnits ( AIC , OwnedPawn , FinalTarget , FinalTypeInt );
		FindFortress ( AIC , FinalTarget , FinalTypeInt );
	}

	if (BB->GetValueAsObject ( TargetActorKey.SelectedKeyName ) != FinalTarget)
	{
		BB->SetValueAsObject ( TargetActorKey.SelectedKeyName , FinalTarget );
		if (FinalTarget)
		{
			BB->SetValueAsInt ( TagKey.SelectedKeyName , FinalTypeInt );
		}
		else
		{
			BB->ClearValue ( TagKey.SelectedKeyName );
		}
	}
}

bool UP_BTS_SelectTarget::HasGameplayTag ( AActor* Actor , FGameplayTag Tag ) const
{
	if (!Actor) return false;

	const IGameplayTagAssetInterface* TagAsset = Cast<IGameplayTagAssetInterface> ( Actor );
	if (TagAsset)
	{
		return TagAsset->HasMatchingGameplayTag ( Tag );
	}

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface> ( Actor );
	if (!ASI) return false;
	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent ();
	if (!ASC) return false;
	return ASC->HasMatchingGameplayTag ( Tag );
}

void UP_BTS_SelectTarget::FindObstacle ( AP_AIControllerEnemyBase* AIC , APawn* OwnedPawn , AActor*& FinalTarget , int32& FinalTypeInt )
{
	if (FinalTarget) return;

	UAIPerceptionComponent* PerceptionComp = AIC->FindComponentByClass<UAIPerceptionComponent> ();
	if (!PerceptionComp) return;

	TArray<AActor*> PerceivedActors;
	PerceptionComp->GetKnownPerceivedActors ( nullptr , PerceivedActors );

	float ClosestDistSq = ObstacleCheckRadius * ObstacleCheckRadius;
	FVector OwnLocation = OwnedPawn->GetActorLocation ();

	for (AActor* Actor : PerceivedActors)
	{
		if (!IsValid ( Actor )) continue;

		if (!HasGameplayTag ( Actor , ObstacleTag )) continue;

		float DistSq = FVector::DistSquared ( OwnLocation , Actor->GetActorLocation () );
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			FinalTarget = Actor;
			FinalTypeInt = 2;
		}
	}
}

void UP_BTS_SelectTarget::FindUnits ( AP_AIControllerEnemyBase* AIC , APawn* OwnedPawn , AActor*& FinalTarget , int32& FinalTypeInt )
{
	if (FinalTarget) return;

	UAIPerceptionComponent* PerceptionComp = AIC->FindComponentByClass<UAIPerceptionComponent> ();
	if (!PerceptionComp) return;

	TArray<AActor*> PerceivedActors;
	PerceptionComp->GetKnownPerceivedActors ( nullptr , PerceivedActors ); // 시각+청각 등 모든 감각

	float ClosestDistSq = FLT_MAX;
	FVector OwnLocation = OwnedPawn->GetActorLocation ();

	for (AActor* Actor : PerceivedActors)
	{
		if (!HasGameplayTag ( Actor , UnitTag )) continue;

		float DistSq = FVector::DistSquared ( OwnLocation , Actor->GetActorLocation () );
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			FinalTarget = Actor;
			FinalTypeInt = 1;
		}
	}
}

void UP_BTS_SelectTarget::FindFortress ( AP_AIControllerEnemyBase* AIC , AActor*& FinalTarget , int32& FinalTypeInt )
{
	if (FinalTarget) return;

	FinalTarget = AIC->GetCachedFortress ();
	if (FinalTarget)
	{
		FinalTypeInt = 3;
	}
}
