#include "PJB/AI/Service/P_BTS_SelectTarget.h"

#include "AIController.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/OverlapResult.h"
#include "PJB/System/P_GameStateBase.h"

UP_BTS_SelectTarget::UP_BTS_SelectTarget ()
{
	NodeName = TEXT ( "Select Target" );
	Interval = 0.5f;
}

void UP_BTS_SelectTarget::TickNode ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds )
{
	Super::TickNode ( OwnerComp , NodeMemory , DeltaSeconds );

	AAIController* AIC = OwnerComp.GetAIOwner ();
	APawn* OwnedPawn = AIC ? AIC->GetPawn () : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent ();
	if (!AIC || !OwnedPawn || !BB) return;

	UAIPerceptionComponent* PerceptionComp = AIC->FindComponentByClass<UAIPerceptionComponent> ();

	AActor* FinalTarget = nullptr;

	// Unit Targetting
	if (PerceptionComp)
	{
		TArray<AActor*> PerceivedActors;
		PerceptionComp->GetKnownPerceivedActors ( nullptr , PerceivedActors ); // 시각+청각 등 모든 감각

		float ClosestDistSq = FLT_MAX;

		for (AActor* Actor : PerceivedActors)
		{
			if (HasGameplayTag ( Actor , UnitTag ))
			{
				float DistSq = OwnedPawn->GetSquaredDistanceTo ( Actor );
				if (DistSq < ClosestDistSq)
				{
					ClosestDistSq = DistSq;
					FinalTarget = Actor;
					BB->SetValueAsInt ( TagKey.SelectedKeyName , 1 );
				}
			}
		}
	}

	// Obstacle Targetting
	if (FinalTarget == nullptr)
	{
		// 내 주변 아주 가까운 곳(SearchRadius)에 장애물이 있는지 검사
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor ( OwnedPawn );

		bool bHit = GetWorld ()->OverlapMultiByObjectType (
			Overlaps ,
			OwnedPawn->GetActorLocation () ,
			FQuat::Identity ,
			FCollisionObjectQueryParams ( FCollisionObjectQueryParams::AllDynamicObjects ) ,
			FCollisionShape::MakeSphere ( ObstacleCheckRadius ) ,
			Params
		);

		if (bHit)
		{
			for (const FOverlapResult& Result : Overlaps)
			{
				AActor* Actor = Result.GetActor ();
				if (HasGameplayTag ( Actor , ObstacleTag ))
				{
					// 장애물 발견! (가장 먼저 걸린 놈 타겟)
					FinalTarget = Actor;
					BB->SetValueAsInt ( TagKey.SelectedKeyName , 2 );
					break;
				}
			}
		}
	}

	// Fortress Targetting
	if (FinalTarget == nullptr)
	{
		if (AP_GameStateBase* GS = GetWorld ()->GetGameState<AP_GameStateBase> ())
		{
			FinalTarget = GS->GetFortress ();
			BB->SetValueAsInt ( TagKey.SelectedKeyName , 3 );
		}
	}

	BB->SetValueAsObject ( TargetActorKey.SelectedKeyName , FinalTarget );
}

bool UP_BTS_SelectTarget::HasGameplayTag ( AActor* Actor , FGameplayTag Tag ) const
{
	if (!Actor) return false;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface> ( Actor );
	if (!ASI) return false;

	UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent ();
	if (!ASC) return false;

	return ASC->HasMatchingGameplayTag ( Tag );
}
