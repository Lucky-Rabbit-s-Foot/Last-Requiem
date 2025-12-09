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
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
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

	APawn* MyPawn = GetPawn ();
	if (MyPawn && SightConfig)
	{
		FVector Center = MyPawn->GetActorLocation ();
		FVector Forward = MyPawn->GetActorForwardVector ();
		Center.Z += 80.0f;


		float Radius = SightConfig->SightRadius;
		float AngleDegree = SightConfig->PeripheralVisionAngleDegrees;

		float AngleRadian = FMath::DegreesToRadians(AngleDegree);


		DrawDebugCone(
			GetWorld(),
			Center,
			Forward,
			Radius,
			AngleRadian,
			0.05f,      
			32,
			FColor::Green,
			false,
			-1.0f,
			0,
			2.0f
		);
	}

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

void AB_UnitAIController::OnTargetDetected(AActor* InActor, FAIStimulus InStimulus)
{
    APawn* MyPawn = GetPawn();
	if (MyPawn == nullptr)
	{
		return;
	}

    if (InStimulus.WasSuccessfullySensed())
    {
        
        DrawDebugSphere(GetWorld(), InActor->GetActorLocation(), 30.0f, 12, FColor::Green, false, 2.0f);

        DrawDebugString(GetWorld(), FVector(0,0,50), TEXT("Enemy Found!"), InActor, FColor::Green, 2.0f);

        UE_LOG(LogTemp, Warning, TEXT("적 발견 : %s"), *InActor->GetName());
        if(BlackboardComponent) 
		{
			BlackboardComponent->SetValueAsObject(TEXT("TargetEnemy"), InActor);
		}
    }
    else
    {
        DrawDebugSphere(GetWorld(), InActor->GetActorLocation(), 30.0f, 12, FColor::Red, false, 2.0f);

        DrawDebugString(GetWorld(), FVector(0,0,50), TEXT("Lost Target..."), InActor, FColor::Red, 2.0f);

        UE_LOG(LogTemp, Warning, TEXT("적 놓침 : %s"), *InActor->GetName());
    }
}
