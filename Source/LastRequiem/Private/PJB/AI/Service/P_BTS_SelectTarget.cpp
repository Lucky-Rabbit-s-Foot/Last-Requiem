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

UP_BTS_SelectTarget::UP_BTS_SelectTarget ()
{
	NodeName = TEXT ( "Select Target" );
	Interval = 0.5f;

	UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
	ObstacleTag = FGameplayTag::RequestGameplayTag ( FName ( "Obstacle" ) );
	FortressTag = FGameplayTag::RequestGameplayTag ( FName ( "Fortress" ) );
}

void UP_BTS_SelectTarget::TickNode ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory , float DeltaSeconds )
{
	Super::TickNode ( OwnerComp , NodeMemory , DeltaSeconds );

	AP_AIControllerEnemyBase* AIC = Cast<AP_AIControllerEnemyBase> ( OwnerComp.GetAIOwner () );
	APawn* OwnedPawn = AIC ? AIC->GetPawn () : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent ();
	if (!AIC || !OwnedPawn || !BB) return;

	AActor* FinalTarget = nullptr;
	int32 FinalTypeInt = 0;

	UAIPerceptionComponent* PerceptionComp = AIC->FindComponentByClass<UAIPerceptionComponent> ();
	if (PerceptionComp)
	{
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

	if (!FinalTarget)
	{
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
			float MinDistSq = FLT_MAX;
			FVector OwnLocation = OwnedPawn->GetActorLocation ();

			for (const FOverlapResult& Result : Overlaps)
			{
				AActor* Actor = Result.GetActor ();
				if (!IsValid ( Actor ) || !HasGameplayTag ( Actor , ObstacleTag )) continue;
			
				float DistSq = FVector::DistSquared ( OwnLocation , Actor->GetActorLocation () );
				if(DistSq < MinDistSq)
				{
					MinDistSq = DistSq;
					FinalTarget = Actor;
					FinalTypeInt = 2;
				}
			}
		}
	}

	if (!FinalTarget)
	{
		FinalTarget = AIC->GetCachedFortress ();
		//LOG_MESSAGE ( Warning , TEXT ( "Selecting Fortress as Target : %s" ) , FinalTarget ? *FinalTarget->GetName () : nullptr );
		if (FinalTarget)
		{
			FinalTypeInt = 3;
		}
	}

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
