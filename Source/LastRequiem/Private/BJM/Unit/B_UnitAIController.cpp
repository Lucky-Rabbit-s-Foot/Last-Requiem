// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/Unit/B_UnitAIController.h"
#include "BJM/Unit/B_UnitBase.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Engine/OverlapResult.h"
#include "Engine/EngineTypes.h"


AB_UnitAIController::AB_UnitAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	if (SightConfig)
	{
		SightConfig->SightRadius = 1000.0f;
		
		SightConfig->LoseSightRadius = 1100.0f;
		
		SightConfig->PeripheralVisionAngleDegrees = 180.0f;

		// 감지 대상 설정 (적, 아군, 중립 모두 감지)
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		// Perception 컴포넌트에 시야 설정 등록
		AIPerception->ConfigureSense(*SightConfig);
		AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
	}

	PrimaryActorTick.bCanEverTick = true;
}

void AB_UnitAIController::Tick(float DeltaTime)
{
	Super::Tick ( DeltaTime );

	if (!BlackboardComponent) return;

	EUnitCommandType CurrentCommand = (EUnitCommandType)BlackboardComponent->GetValueAsEnum ( TEXT ( "Command" ) );

	// Move가 아닐 때만(Idle, AttackMove, Hold 등) 시야를 그림
	if (CurrentCommand != EUnitCommandType::Move)
	{
		APawn* MyPawn = GetPawn ();
		if (MyPawn && SightConfig)
		{
			FVector Center = MyPawn->GetActorLocation ();
			FVector Forward = MyPawn->GetActorForwardVector ();
			Center.Z += 80.0f;


			float Radius = SightConfig->SightRadius;
			float AngleDegree = SightConfig->PeripheralVisionAngleDegrees;

			float AngleRadian = FMath::DegreesToRadians ( AngleDegree );


			DrawDebugCone (
				GetWorld () ,
				Center ,
				Forward ,
				Radius ,
				AngleRadian ,
				0.05f ,
				128 ,
				FColor::Green ,
				false ,
				-1.0f ,
				0 ,
				2.0f
			);
		}
	}
	CheckNearbyEnemies ();

}

void AB_UnitAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AB_UnitAIController::OnTargetDetected);

	if (BTAsset && BlackboardComponent)
	{
		BlackboardComponent->InitializeBlackboard(*BTAsset->BlackboardAsset);
		RunBehaviorTree(BTAsset);
	}
}

void AB_UnitAIController::CheckNearbyEnemies ()
{
	if (!BlackboardComponent || !AIPerception) return;

	EUnitCommandType CurrentCommand = (EUnitCommandType)BlackboardComponent->GetValueAsEnum ( TEXT ( "Command" ) );
	if (CurrentCommand == EUnitCommandType::Move)
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor> ( BlackboardComponent->GetValueAsObject ( TEXT ( "TargetEnemy" ) ) );
	if (CurrentTarget)
	{
		float Dist = FVector::Dist ( GetPawn ()->GetActorLocation () , CurrentTarget->GetActorLocation () );
		if (Dist > 1200.0f)
		{
			BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
			ClearFocus ( EAIFocusPriority::Gameplay );
		}
		return;
	}

	TArray<AActor*> PerceivedActors;
	AIPerception->GetKnownPerceivedActors ( nullptr , PerceivedActors );

	AActor* ClosestEnemy = nullptr;
	float MinDistance = FLT_MAX;
	APawn* MyPawn = GetPawn ();

	for (AActor* Actor : PerceivedActors)
	{
		AB_UnitBase* EnemyUnit = Cast<AB_UnitBase> ( Actor );

		if (EnemyUnit && EnemyUnit != MyPawn && EnemyUnit->IsAlive ())
		{
			float Dist = FVector::Dist ( MyPawn->GetActorLocation () , EnemyUnit->GetActorLocation () );

			if (Dist < MinDistance)
			{
				MinDistance = Dist;
				ClosestEnemy = EnemyUnit;
			}
		}
	}

	if (ClosestEnemy)
	{
		BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , ClosestEnemy );
		SetFocus ( ClosestEnemy );
		UE_LOG ( LogTemp , Warning , TEXT ( "시야 내 적 포착! 공격 개시: %s" ) , *ClosestEnemy->GetName () );
	}
}

void AB_UnitAIController::OnTargetDetected(AActor* InActor, FAIStimulus InStimulus)
{
	APawn* MyPawn = GetPawn ();
	if (!MyPawn || !BlackboardComponent) return;


	EUnitCommandType CurrentCommand = (EUnitCommandType)BlackboardComponent->GetValueAsEnum ( TEXT ( "Command" ) );

	if (CurrentCommand == EUnitCommandType::Move)
	{
		return;
	}

    if (InStimulus.WasSuccessfullySensed())
    {
        
		DrawDebugSphere ( GetWorld () , InActor->GetActorLocation () , 30.0f , 12 , FColor::Green , false , 2.0f );
		UE_LOG ( LogTemp , Warning , TEXT ( "시야: 적 발견 %s" ) , *InActor->GetName () );

		BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , InActor );
		SetFocus ( InActor );
    }
    else
    {
		UE_LOG ( LogTemp , Warning , TEXT ( "시야: 적 놓침 %s" ) , *InActor->GetName () );

		UObject* CurrentTarget = BlackboardComponent->GetValueAsObject ( TEXT ( "TargetEnemy" ) );
		if (CurrentTarget == InActor)
		{
			BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
			ClearFocus ( EAIFocusPriority::Gameplay );
		}
    }
}

void AB_UnitAIController::SetCommandState ( EUnitCommandType NewCommand , FVector TargetLocation )
{
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)NewCommand );

		BlackboardComponent->SetValueAsVector ( TEXT ( "TargetLocation" ) , TargetLocation );

		if (NewCommand == EUnitCommandType::None || NewCommand == EUnitCommandType::Hold)
		{
			BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
		}
	}

}


