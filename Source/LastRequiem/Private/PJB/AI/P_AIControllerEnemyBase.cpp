#include "PJB/AI/P_AIControllerEnemyBase.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"

#include "Navigation/CrowdFollowingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameplayTagAssetInterface.h"
#include "Kismet/GameplayStatics.h"

#include "PJB/System/P_GameStateBase.h"

AP_AIControllerEnemyBase::AP_AIControllerEnemyBase (const FObjectInitializer& ObjectInitializer)
	: Super ( ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent> ( TEXT ( "PathFollowingComponent" ) ) )
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent> ( TEXT ( "AIPerception" ) );
	SetPerceptionComponent ( *AIPerceptionComp );
	SightConfig_v2 = CreateDefaultSubobject<UAISenseConfig_Sight> ( TEXT ( "SenseSight" ) );

	InitSightConfig ();
	InitPerceptionComponent ();
}

void AP_AIControllerEnemyBase::SetCachedFortressByGameState ()
{
	if (!GetWorld ()) return;
	if(AP_GameStateBase* GS = GetWorld ()->GetGameState<AP_GameStateBase> ())
	{
		CachedFortress = GS->GetFortress ();
	}
}

void AP_AIControllerEnemyBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AP_AIControllerEnemyBase::OnPossess ( APawn* InPawn )
{
	Super::OnPossess ( InPawn );

	InitSightConfig ();
	InitPerceptionComponent ();

	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent> ( GetPathFollowingComponent () ))
	{
		CrowdComp->SetCrowdSeparationWeight ( 20.0f , true );
		CrowdComp->SetCrowdAvoidanceQuality ( ECrowdAvoidanceQuality::High );
		CrowdComp->SetCrowdCollisionQueryRange ( 400.0f );
		CrowdComp->SetCrowdAvoidanceRangeMultiplier ( 1.0f );
	}
	SetCachedFortressByGameState ();

	if (BehaviorTree)
	{
		RunBehaviorTree ( BehaviorTree );
	}
}

void AP_AIControllerEnemyBase::InitPerceptionComponent ()
{
	if (!AIPerceptionComp || !SightConfig_v2) return;

	AIPerceptionComp->ConfigureSense ( *SightConfig_v2 );
	AIPerceptionComp->SetDominantSense ( SightConfig_v2->GetSenseImplementation () );
}

void AP_AIControllerEnemyBase::InitSightConfig ()
{
	if (!SightConfig_v2) return;

	SightConfig_v2->SightRadius = SightRadius;
	SightConfig_v2->LoseSightRadius = LoseSightRadius;
	SightConfig_v2->PeripheralVisionAngleDegrees = ViewAngleDegree;

	SightConfig_v2->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig_v2->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig_v2->DetectionByAffiliation.bDetectNeutrals = true;
}
