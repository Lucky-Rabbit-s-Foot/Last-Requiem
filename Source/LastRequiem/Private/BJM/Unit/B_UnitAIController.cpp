// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/Unit/B_UnitAIController.h"
#include "BJM/Unit/B_UnitBase.h"
#include "BJM/Unit/B_UnitStatusComponent.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Engine/OverlapResult.h"
#include "Engine/EngineTypes.h"
#include "PJB/Enemy/P_EnemyBase.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagsManager.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

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


			//DrawDebugCone (
			//	GetWorld () ,
			//	Center ,
			//	Forward ,
			//	Radius ,
			//	AngleRadian ,
			//	0.005f ,
			//	50 ,
			//	FColor::Green ,
			//	false ,
			//	-1.0f ,
			//	0 ,
			//	2.0f
			//);
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
	APawn* MyPawn = GetPawn ();
	if (!IsValid ( MyPawn ))
	{
		return;
	}

	if (!BlackboardComponent || !AIPerception) return;

	AB_UnitBase* MyUnitBase = Cast<AB_UnitBase> ( MyPawn );
	if (!MyUnitBase || !MyUnitBase->StatusComponent) return;

	if (MyUnitBase->StatusComponent->CurrentState == EUnitBehaviorState::Panic)
	{
		BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
		ClearFocus ( EAIFocusPriority::Gameplay );
		MyUnitBase->SetCombatState_Unit ( false );

		if (GetMoveStatus () == EPathFollowingStatus::Idle)
		{
			FNavLocation RandomNavLoc;
			UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1> ( GetWorld () );

			if (NavSystem && NavSystem->GetRandomReachablePointInRadius ( MyPawn->GetActorLocation () , 300.0f , RandomNavLoc ))
			{
				BlackboardComponent->SetValueAsVector ( TEXT ( "TargetLocation" ) , RandomNavLoc.Location );
				BlackboardComponent->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::Move );

				UE_LOG ( LogTemp , Warning , TEXT ( "[%s] 패닉 상태! 제멋대로 이동 중..." ) , *MyPawn->GetName () );
			}
		}

		return;
	}




	EUnitCommandType CurrentCommand = (EUnitCommandType)BlackboardComponent->GetValueAsEnum ( TEXT ( "Command" ) );

	if (CurrentCommand == EUnitCommandType::Move || CurrentCommand == EUnitCommandType::Stop || CurrentCommand == EUnitCommandType::Retreat)
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor> ( BlackboardComponent->GetValueAsObject ( TEXT ( "TargetEnemy" ) ) );

	if (IsValid ( CurrentTarget ))
	{
		AB_UnitBase* TargetUnit = Cast<AB_UnitBase> ( CurrentTarget );
		if (TargetUnit && !TargetUnit->IsAlive ())
		{
			// 아군 타겟이 죽었으면 타겟 해제
			BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
			ClearFocus ( EAIFocusPriority::Gameplay );

			if (AB_UnitBase* MyPawnUnit = Cast<AB_UnitBase> ( GetPawn () ))
			{
				MyPawnUnit->SetCombatState_Unit ( false );
			}
			return;
		}

		float Dist = FVector::Dist ( MyPawn->GetActorLocation () , CurrentTarget->GetActorLocation () );

		if (Dist > 1200.0f)
		{
			BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
			ClearFocus ( EAIFocusPriority::Gameplay );

			//if (AB_UnitBase* MyUnit = Cast<AB_UnitBase> ( MyPawn ))
			//{
			//	MyUnit->SetCombatState_Unit ( false );
			//}

			UE_LOG ( LogTemp , Warning , TEXT ( "타겟이 너무 멀어짐 -> 추격 포기" ) );
		}
		return;
	}

	bool bIsMadness = false;

	if (MyUnitBase && MyUnitBase->StatusComponent)
	{
		if (MyUnitBase->StatusComponent->CurrentState == EUnitBehaviorState::Madness)
		{
			bIsMadness = true;
		}
	}

	float ScanRadius = 1000.0f;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams ( NAME_None , false , MyPawn );

	bool bResult = GetWorld ()->OverlapMultiByChannel (
		OverlapResults ,
		MyPawn->GetActorLocation () ,
		FQuat::Identity ,
		ECollisionChannel::ECC_Pawn ,
		FCollisionShape::MakeSphere ( ScanRadius ) ,
		QueryParams
	);

	AActor* ClosestTarget = nullptr;
	float MinDistance = FLT_MAX;

	FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag ( FName ( "Enemy" ) );
	FGameplayTag UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );

	for (auto const& OverlapResult : OverlapResults)
	{
		AActor* HitActor = OverlapResult.GetActor ();
		if (!HitActor || HitActor == MyPawn) continue;
		//if (!HitActor) continue;
		//if (HitActor == MyPawn) continue;

		AB_UnitBase* HitUnit = Cast<AB_UnitBase> ( HitActor );
		if (HitUnit && !HitUnit->IsAlive ())
		{
			continue;
		}

		IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface> ( HitActor );

		if (TagInterface)
		{
			FGameplayTagContainer OwnedTags;
			TagInterface->GetOwnedGameplayTags ( OwnedTags );

			bool bIsValidTarget = false;

			if (OwnedTags.HasTag ( EnemyTag ))
			{
				bIsValidTarget = true;
			}
			else if (bIsMadness && OwnedTags.HasTag ( UnitTag ))
			{
				bIsValidTarget = true;
			}

			if (bIsValidTarget)
			{
				float Dist = FVector::Dist ( MyPawn->GetActorLocation () , HitActor->GetActorLocation () );
				if (Dist < MinDistance)
				{
					MinDistance = Dist;
					ClosestTarget = HitActor;
				}
			}
		}
	}

	// 최종 타겟 설정
	if (ClosestTarget)
	{
		BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , ClosestTarget );
		SetFocus ( ClosestTarget );

		if (MyUnitBase)
		{
			if (MyUnitBase->StatusComponent && !MyUnitBase->StatusComponent->bIsInCombat)
			{
				MyUnitBase->PlayVoiceForEvent ( EUnitVoiceEvent::SpotEnemy );
			}

			MyUnitBase->SetCombatState_Unit ( true );

			if (MyUnitBase->StatusComponent)
			{
				EUnitBehaviorState State = MyUnitBase->StatusComponent->CurrentState;

				// 패닉(돌진만) or 광기(팀킬돌진) 상태라면?
				if (State == EUnitBehaviorState::Panic || State == EUnitBehaviorState::Madness)
				{

					BlackboardComponent->SetValueAsVector ( TEXT ( "TargetLocation" ) , ClosestTarget->GetActorLocation () );

					BlackboardComponent->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::AttackMove );

				}
			}
		}

		IGameplayTagAssetInterface* TargetTagInterface = Cast<IGameplayTagAssetInterface> ( ClosestTarget );
		FGameplayTagContainer TargetTags;
		TargetTagInterface->GetOwnedGameplayTags ( TargetTags );

		if (TargetTags.HasTag ( UnitTag ))
		{
			UE_LOG ( LogTemp , Error , TEXT ( "광기 아군(%s)을 적으로 인식" ) , *ClosestTarget->GetName () );
		}
		else
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "적(%s) 발견! 공격 개시" ) , *ClosestTarget->GetName () );
		}
	}
	else
	{
		if (MyUnitBase)
		{
			MyUnitBase->SetCombatState_Unit ( false );
		}
	}
}

void AB_UnitAIController::OnTargetDetected(AActor* InActor, FAIStimulus InStimulus)
{
	APawn* MyPawn = GetPawn ();
	if (!MyPawn || !BlackboardComponent) return;


	EUnitCommandType CurrentCommand = (EUnitCommandType)BlackboardComponent->GetValueAsEnum ( TEXT ( "Command" ) );

	if (CurrentCommand == EUnitCommandType::Move || CurrentCommand == EUnitCommandType::Stop || CurrentCommand == EUnitCommandType::Retreat)
	{
		return;
	}

    if (InStimulus.WasSuccessfullySensed())
    {
		IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface> ( InActor );
		if (TagInterface)
		{
			FGameplayTagContainer OwnedTags;
			TagInterface->GetOwnedGameplayTags ( OwnedTags );
			FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag ( FName ( "Enemy" ) );

			if (!OwnedTags.HasTag ( EnemyTag ))
			{
				return;
			}

			if (AB_UnitBase* MyUnit = Cast<AB_UnitBase> ( GetPawn () ))
			{
				if (!MyUnit->StatusComponent->bIsInCombat)
				{
					MyUnit->PlayVoiceForEvent ( EUnitVoiceEvent::SpotEnemy );
				}

				MyUnit->SetCombatState_Unit ( true );
			}

		}
		else
		{
			return;
		}
		//DrawDebugSphere ( GetWorld () , InActor->GetActorLocation () , 30.0f , 12 , FColor::Green , false , 2.0f );
		UE_LOG ( LogTemp , Warning , TEXT ( "시야: 적 발견 %s" ) , *InActor->GetName () );

		BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , InActor );
		SetFocus ( InActor );

		if (AB_UnitBase* MyUnit = Cast<AB_UnitBase> ( GetPawn () ))
		{
			MyUnit->SetCombatState_Unit ( true );
		}
    }
    else
    {
		UE_LOG ( LogTemp , Warning , TEXT ( "시야: 적 놓침 %s" ) , *InActor->GetName () );

		UObject* CurrentTarget = BlackboardComponent->GetValueAsObject ( TEXT ( "TargetEnemy" ) );
		if (CurrentTarget == InActor)
		{
			BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
			ClearFocus ( EAIFocusPriority::Gameplay );

			if (AB_UnitBase* MyUnit = Cast<AB_UnitBase> ( GetPawn () ))
			{
				MyUnit->SetCombatState_Unit ( false );
			}

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

			if (AB_UnitBase* MyUnit = Cast<AB_UnitBase> ( GetPawn () ))
			{
				MyUnit->SetCombatState_Unit ( false );
			}
		}
		if (NewCommand == EUnitCommandType::Stop || NewCommand == EUnitCommandType::Move)   
		{
			BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
			ClearFocus ( EAIFocusPriority::Gameplay );

			if (AB_UnitBase* MyUnit = Cast<AB_UnitBase> ( GetPawn () ))
			{
				MyUnit->SetCombatState_Unit ( false );
			}
		}
	}
}

void AB_UnitAIController::OnMoveCompleted ( FAIRequestID RequestID , const FPathFollowingResult& Result )
{
	Super::OnMoveCompleted ( RequestID , Result );

	if (!BlackboardComponent) return;

	EUnitCommandType CurrentCommand = (EUnitCommandType)BlackboardComponent->GetValueAsEnum ( TEXT ( "Command" ) );

	if (CurrentCommand == EUnitCommandType::Retreat)
	{
		BlackboardComponent->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::Hold );

		BlackboardComponent->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );

		UE_LOG ( LogTemp , Warning , TEXT ( "후퇴 완료 Hold 상태로 전환" ) );
	}
}


