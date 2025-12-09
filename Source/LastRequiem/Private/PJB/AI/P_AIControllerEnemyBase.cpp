#include "PJB/AI/P_AIControllerEnemyBase.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Kismet/GameplayStatics.h"

// include Unit Header
// include Fortress Header

AP_AIControllerEnemyBase::AP_AIControllerEnemyBase ()
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent> ( TEXT ( "AIPerception" ) );
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight> ( TEXT ( "SightConfig" ) );
	if (SightConfig)
	{
		SightConfig->SightRadius = SightRadius;
		SightConfig->LoseSightRadius = LoseSightRadius;
		SightConfig->PeripheralVisionAngleDegrees = ViewAngleDegree;

		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		AIPerceptionComp->ConfigureSense ( *SightConfig );
	}

	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage> ( TEXT ( "DamageConfig" ) );
	if (DamageConfig)
	{
		//DamageConfig->SetImplementation ( UAISenseConfig_Damage::StaticClass () );
		AIPerceptionComp->ConfigureSense ( *DamageConfig );
	}

}

void AP_AIControllerEnemyBase::OnPossess ( APawn* InPawn )
{
	Super::OnPossess ( InPawn );

	if (BehaviorTree)
	{
		RunBehaviorTree ( BehaviorTree );
	}
	UpdateBestTarget ();
}

void AP_AIControllerEnemyBase::OnTargetPerceived ( AActor* InActor , FAIStimulus Stimulus )
{
	UBlackboardComponent* BB = GetBlackboardComponent ();
	if (!BB) return;

	if (Stimulus.Type == UAISense::GetSenseID<UAISenseConfig_Damage> ())
	{
		if (Stimulus.WasSuccessfullySensed ())
		{
			/*
			Unit 에서 추가해야 할 코드
			#include "Perception/AISense_Damage.h"

			UAISense_Damage::ReportDamageEvent(
				GetWorld(),
				EnemyActor,                     // 맞은 놈 (적 AI)
				this,                           // 때린 놈 (플레이어)
				10.0f,                          // 데미지 양
				this->GetActorLocation(),       // 때린 위치
				EnemyActor->GetActorLocation()  // 맞은 위치
			);

			*/


			BB->SetValueAsObject ( TEXT ( "TargetActor" ) , InActor );
			//BB->SetValueAsEnum ( TEXT ( "TargetType" ) , 1 );
			


			SetFocus ( InActor );
			return;
		}
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight> ())
	{
		UpdateBestTarget ();
	}
}

void AP_AIControllerEnemyBase::UpdateBestTarget ()
{
	UBlackboardComponent* BB = GetBlackboardComponent ();
	if (!BB)
	{
		return;
	}

	APawn* PossedPawn = GetPawn ();
	if (!PossedPawn)
	{
		return;
	}

	AActor* ClosestUnit = nullptr;
	float MinDistSq = FLT_MAX;

	TArray<AActor*> TargetCandidates;

	TArray<AActor*> VisibleActors;
	AIPerceptionComp->GetCurrentlyPerceivedActors ( UAISenseConfig_Sight::StaticClass () , VisibleActors );
	for (AActor* VisibleActor : VisibleActors)
	{
		if (VisibleActor)
		{
			// AActor -> Unit Class
			if (AActor* PerceiveUnit = Cast<AActor> ( VisibleActor ))
			{
				float DistSq = PossedPawn->GetSquaredDistanceTo (VisibleActor);
				if (DistSq < MinDistSq)
				{
					MinDistSq = DistSq;
					ClosestUnit = VisibleActor;
				}
			}
		}
	}

	TArray<AActor*> DamagingActors;
	AIPerceptionComp->GetCurrentlyPerceivedActors ( UAISenseConfig_Damage::StaticClass () , DamagingActors );
	for (AActor* DamagingActor : DamagingActors)
	{
		if (DamagingActor)
		{
			// AActor -> Unit Class
			if (AActor* PerceiveUnit = Cast<AActor> ( DamagingActor ))
			{
				float DistSq = PossedPawn->GetSquaredDistanceTo ( DamagingActor );
				if (DistSq < MinDistSq)
				{
					MinDistSq = DistSq;
					ClosestUnit = DamagingActor;
				}
			}
		}
	}

	if (ClosestUnit)
	{
		BB->SetValueAsObject ( TEXT ( "TargetActor" ), ClosestUnit );
		//BB->SetValueAsEnum ( TEXT ( "TargetType" ) , 1 );
	}
	else
	{
		TArray<AActor*> Fortresses;
		UGameplayStatics::GetAllActorsWithTag ( this , TEXT ( "Fortress" ) , Fortresses );

		if (Fortresses.Num () > 0)
		{
			BB->SetValueAsObject ( TEXT ( "TargetActor" ) , Fortresses[0]);
			//BB->SetValueAsEnum ( TEXT ( "TragetType" ) , 2 );
		}
		else
		{
			BB->SetValueAsObject ( TEXT ( "TargetActor" ) , nullptr );
			//BB->SetValueAsEnum ( TEXT ( "TargetType" ) , 0 );
		}
	}
}
