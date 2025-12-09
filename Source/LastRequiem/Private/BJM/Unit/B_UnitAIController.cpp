// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/Unit/B_UnitAIController.h"
#include "BJM/Unit/B_UnitBase.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AB_UnitAIController::AB_UnitAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	if (SightConfig)
	{
		SightConfig->SightRadius = 1500.0f;
		
		SightConfig->LoseSightRadius = 2000.0f;
		
		SightConfig->PeripheralVisionAngleDegrees = 60.0f;

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

	// 1. 내 몸체(Pawn)가 있고, 시야 설정(SightConfig)이 있을 때만 그림
	APawn* MyPawn = GetPawn ();
	if (MyPawn && SightConfig)
	{
		// 2. 그릴 위치와 방향 계산
		FVector Center = MyPawn->GetActorLocation (); // 유닛 위치
		FVector Forward = MyPawn->GetActorForwardVector (); // 유닛이 보는 방향
		Center.Z += 80.0f;


		float Radius = SightConfig->SightRadius;
		float AngleDegree = SightConfig->PeripheralVisionAngleDegrees;

		float AngleRadian = FMath::DegreesToRadians ( AngleDegree );


		DrawDebugCone (
			GetWorld () ,
			Center ,
			Forward ,
			Radius ,
			AngleRadian , // [가로] 시야각 (90도) - 이건 그대로 유지!
			0.05f ,       // [세로] 높이 - ❌AngleRadian 대신 아주 작은 숫자(0.05f)를 넣어!
			32 ,
			FColor::Green ,
			false ,
			-1.0f ,
			0 ,
			2.0f
		);
	}

}

void AB_UnitAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AB_UnitAIController::OnTargetDetected);

	// BT들어가야됨
	// if (BTAsset) RunBehaviorTree(BTAsset);
}

void AB_UnitAIController::OnTargetDetected(AActor* InActor, FAIStimulus InStimulus)
{
// 내 몸체(Pawn)가 없으면 그림을 못 그리니까 안전장치
    APawn* MyPawn = GetPawn();
    if (MyPawn == nullptr) return;

    if (InStimulus.WasSuccessfullySensed())
    {
        // 1. [발견함!] -> 초록색(Green) 그림
        
        // (1) 적 위치에 초록색 공 그리기
        DrawDebugSphere(GetWorld(), InActor->GetActorLocation(), 30.0f, 12, FColor::Green, false, 2.0f);
        
        // (2) 나(유닛) -> 적(Enemy) 사이를 잇는 초록색 레이저 쏘기
       // DrawDebugLine(GetWorld(), MyPawn->GetActorLocation(), InActor->GetActorLocation(), FColor::Green, false, 2.0f, 0, 3.0f);

        // (3) 머리 위에 글씨 띄우기
        DrawDebugString(GetWorld(), FVector(0,0,50), TEXT("Enemy Found!"), InActor, FColor::Green, 2.0f);

        // 로그 및 블랙보드 처리 (기존 코드)
        UE_LOG(LogTemp, Warning, TEXT("적 발견 : %s"), *InActor->GetName());
        if (BlackboardComponent) BlackboardComponent->SetValueAsObject(TEXT("TargetEnemy"), InActor);
    }
    else
    {
        // 2. [놓침!] -> 빨간색(Red) 그림

        // (1) 적 위치에 빨간색 공 그리기
        DrawDebugSphere(GetWorld(), InActor->GetActorLocation(), 30.0f, 12, FColor::Red, false, 2.0f);

        // (2) 글씨 띄우기
        DrawDebugString(GetWorld(), FVector(0,0,50), TEXT("Lost Target..."), InActor, FColor::Red, 2.0f);

        UE_LOG(LogTemp, Warning, TEXT("적 놓침 : %s"), *InActor->GetName());
    }
}
