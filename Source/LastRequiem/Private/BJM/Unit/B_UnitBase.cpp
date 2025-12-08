// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/Unit/B_UnitBase.h"
#include "BJM/Unit/B_UnitStatusComponent.h"
#include "BJM/Unit/B_UnitAIController.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AB_UnitBase::AB_UnitBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatusComponent = CreateDefaultSubobject<UB_UnitStatusComponent>(TEXT("StatusComponent"));

	// [중요] 이 유닛은 우리가 만든 AI 컨트롤러가 조종한다!
	AIControllerClass = AB_UnitAIController::StaticClass ();

	// 유닛이 회전할 때 컨트롤러가 보는 방향으로 몸을 돌리도록 설정
	bUseControllerRotationYaw = false; // 카메라는 따로 돌고
	GetCharacterMovement()->bOrientRotationToMovement = true; // 이동 방향으로 몸을 돌림

}

// Called when the game starts or when spawned
void AB_UnitBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AB_UnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AB_UnitBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AB_UnitBase::CommandMoveToLocation(float InX, float InY)
{
	// 1. 현재 내 컨트롤러 가져오기
	AController* controller = GetController ();

	// 2. 형변환 (우리가 만든 AI 컨트롤러인지 확인)
	AB_UnitAIController* AIController = Cast<AB_UnitAIController> ( controller );

	if (AIController)
	{
		// 3. 목표 위치 만들기 (Z축은 현재 내 높이 그대로)
		FVector TargetLocation ( InX , InY , GetActorLocation ().Z );

		// 4. 이동 명령 내리기 (MoveToLocation은 언리얼 내장 함수)
		// - TargetLocation: 목표 지점
		// - -1.0f: 멈추는 거리 (기본값)
		AIController->MoveToLocation ( TargetLocation );

		// (디버그용) 로그 찍어보기
		// UE_LOG(LogTemp, Warning, TEXT("Moving To: %f, %f"), InX, InY);
	}
}

