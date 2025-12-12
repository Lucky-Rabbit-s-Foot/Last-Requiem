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
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight> ( TEXT ( "SenseSight" ) );
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage> ( TEXT ( "SenseDamage" ) );

	InitSightConfig ();
	InitDamageConfig ();

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

void AP_AIControllerEnemyBase::OnPossess ( APawn* InPawn )
{
	Super::OnPossess ( InPawn );

	if (BehaviorTree)
	{
		RunBehaviorTree ( BehaviorTree );
	}
}

void AP_AIControllerEnemyBase::InitPerceptionComponent ()
{
	if (!AIPerceptionComp) return;

	AIPerceptionComp->ConfigureSense ( *DamageConfig );
	AIPerceptionComp->ConfigureSense ( *SightConfig );
	AIPerceptionComp->SetDominantSense ( SightConfig->GetSenseImplementation () );
}

void AP_AIControllerEnemyBase::InitSightConfig ()
{
	if (!SightConfig) return;

	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = ViewAngleDegree;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
}

void AP_AIControllerEnemyBase::InitDamageConfig ()
{
	if (!DamageConfig) return;
	//DamageConfig->SetImplementation(UAISense_Damage::StaticClass());

}
