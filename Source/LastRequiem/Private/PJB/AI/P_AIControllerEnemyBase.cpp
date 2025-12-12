#include "PJB/AI/P_AIControllerEnemyBase.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameplayTagAssetInterface.h"
#include "Kismet/GameplayStatics.h"

#include "PJB/System/P_GameStateBase.h"

AP_AIControllerEnemyBase::AP_AIControllerEnemyBase ()
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent> ( TEXT ( "AIPerception" ) );
	
	InitSightConfig ();
	InitDamageConfig ();
	
	AIPerceptionComp->SetDominantSense ( SightConfig->GetSenseImplementation () );

	InitGameplayTags ();
}

void AP_AIControllerEnemyBase::SetCachedFortress ()
{
	if (!GetWorld ()) return;
	if (AP_GameStateBase* GS = GetWorld ()->GetGameState<AP_GameStateBase> ())
	{
		CachedFortress = GS->GetFortress ();
	}
}

void AP_AIControllerEnemyBase::InitGameplayTags ()
{
	UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
	ObstacleTag = FGameplayTag::RequestGameplayTag ( FName ( "Obstacle" ) );
	FortressTag = FGameplayTag::RequestGameplayTag ( FName ( "Fortress" ) );
}

void AP_AIControllerEnemyBase::InitDamageConfig ()
{
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage> ( TEXT ( "SenseDamage" ) );
	if (!DamageConfig) return;
	//DamageConfig->SetImplementation ( UAISense_Damage::StaticClass () );
	AIPerceptionComp->ConfigureSense ( *DamageConfig );
}

void AP_AIControllerEnemyBase::InitSightConfig ()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight> ( TEXT ( "SenseSight" ) );
	if (!SightConfig) return;
	SightRangeSetting ();
	SightDetectionSetting ();
	AIPerceptionComp->ConfigureSense ( *SightConfig );
}

void AP_AIControllerEnemyBase::SightRangeSetting ()
{
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = ViewAngleDegree;
}

void AP_AIControllerEnemyBase::SightDetectionSetting ()
{
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
}

void AP_AIControllerEnemyBase::OnPossess ( APawn* InPawn )
{
	Super::OnPossess ( InPawn );

	SetCachedFortress ();
	BindPerceptionUpdate ();
	
	if (BehaviorTree)
	{
		RunBehaviorTree ( BehaviorTree );
	}
	
	UpdateBestTarget ();
}

void AP_AIControllerEnemyBase::BindPerceptionUpdate ()
{
	if (!AIPerceptionComp) return;
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic ( this , &AP_AIControllerEnemyBase::OnTargetPerceived );
}

void AP_AIControllerEnemyBase::OnTargetPerceived ( AActor* InActor , FAIStimulus Stimulus )
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage> ()
		|| Stimulus.Type == UAISense::GetSenseID<UAISense_Sight> ())
	{
		UpdateBestTarget ();
	}
}

void AP_AIControllerEnemyBase::UpdateBestTarget ()
{
	UBlackboardComponent* BB = GetBlackboardComponent ();
	APawn* PossedPawn = GetPawn ();
	if (!BB || !PossedPawn) return;

	AActor* ClosestUnit = nullptr;
	float MinDistSq = FLT_MAX;
	FindClosestUnitOnVisible ( PossedPawn , MinDistSq , ClosestUnit );
	FindClosestUnitOnDamaging ( PossedPawn , MinDistSq , ClosestUnit );

	if (ClosestUnit)
	{
		BB->SetValueAsObject ( TEXT ( "TargetActor" ), ClosestUnit );
		BB->SetValueAsBool ( TEXT ( "bIsTargetUnit" ) , true );
	}
	else
	{
		if(CachedFortress.IsValid())
		{
			BB->SetValueAsObject ( TEXT ( "TargetActor" ) , CachedFortress.Get () );
			BB->ClearValue ( TEXT ( "bIsTargetUnit" ) );
		}
		else
		{
			BB->SetValueAsObject ( TEXT ( "TargetActor" ) , nullptr );
		}
	}
}

void AP_AIControllerEnemyBase::FindClosestUnitOnDamaging ( APawn* PossedPawn , float& MinDistSq , AActor*& ClosestUnit )
{
	TArray<AActor*> DamagingActors;
	AIPerceptionComp->GetCurrentlyPerceivedActors ( UAISense_Damage::StaticClass () , DamagingActors );
	for (AActor* DamagingActor : DamagingActors)
	{
		if (!IsUnit ( DamagingActor )) continue;

		float DistSq = PossedPawn->GetSquaredDistanceTo ( DamagingActor );
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			ClosestUnit = DamagingActor;
		}
	}
}

void AP_AIControllerEnemyBase::FindClosestUnitOnVisible ( APawn* PossedPawn , float& MinDistSq , AActor*& ClosestUnit )
{
	TArray<AActor*> VisibleActors;
	AIPerceptionComp->GetCurrentlyPerceivedActors ( UAISense_Sight::StaticClass () , VisibleActors );
	for (AActor* VisibleActor : VisibleActors)
	{
		if (!IsUnit ( VisibleActor )) continue;

		float DistSq = PossedPawn->GetSquaredDistanceTo ( VisibleActor );
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			ClosestUnit = VisibleActor;
		}
	}
}

bool AP_AIControllerEnemyBase::IsUnit ( AActor* InActor ) const
{
	if (!InActor) return false;

	const IGameplayTagAssetInterface* TagAsset = Cast<IGameplayTagAssetInterface> ( InActor );
	if (!TagAsset) return false;

	return TagAsset->HasMatchingGameplayTag ( UnitTag );
}
