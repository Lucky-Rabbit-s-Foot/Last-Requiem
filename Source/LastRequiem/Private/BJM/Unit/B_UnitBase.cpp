// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/Unit/B_UnitBase.h"
#include "BJM/Unit/B_UnitStatusComponent.h"
#include "BJM/Unit/B_UnitAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KWB/UI/W_MapWidget.h"


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

	if (StatusComponent == nullptr)
	{
		return;
	}

	// 현재 상태 확인
	EUnitBehaviorState CurrentState = StatusComponent->CurrentState;

	switch (CurrentState)
	{
		// 말 잘듣는 상태
		case EUnitBehaviorState::Inspired: // 고양 (충성도 상)
		case EUnitBehaviorState::Cold:	   // 냉정 (충성도 중)
		case EUnitBehaviorState::Normal:   // 노말
	
			ProcessMoveCommand(InX, InY);
			break;

		// 말 건성으로 듣는 상태
		case EUnitBehaviorState::Lazy: // 나태 (충성도 하)
			// 50퍼 확률로 명령 수행
			if (FMath::RandRange(0, 100) > 50)
			{
				ProcessMoveCommand(InX, InY);
			}
			else
			{
				// TODO: 나중에 "아 귀찮게 진짜..." 같은 보이스 출력
				UE_LOG(LogTemp, Warning, TEXT("나태"));
			}
			break;


		// 통제가 힘든 상태
		case EUnitBehaviorState::Madness: // 광기 (충성도 상)
			// TODO: 적에게 무지성 돌진하거나 웃음소리 출력
			UE_LOG(LogTemp, Error, TEXT("광기"));
			break;

		case EUnitBehaviorState::Fear:	  // 공포 (충성도 중)
			// TODO: 뒷걸음질 치거나 제자리에 주저앉음
			UE_LOG(LogTemp, Error, TEXT("공포"));
			break;

		case EUnitBehaviorState::Panic:   // 패닉 (충성도 하)
			// TODO: 아군을 공격하거나 도망침
			UE_LOG(LogTemp, Error, TEXT("패닉"));
			break;
	}
}

void AB_UnitBase::ProcessMoveCommand(float InX, float InY)
{
	// 현재 내 컨트롤러 가져오기
	AController* controller = GetController();

	// 내가 만든 AI 컨트롤러인지 확인
	AB_UnitAIController* AIController = Cast<AB_UnitAIController>(controller);

	if (AIController)
	{
		// 목표 위치 만들기
		FVector TargetLocation(InX, InY, GetActorLocation().Z);

		// 목표 지점
		AIController->MoveToLocation(TargetLocation);


	}
}

void AB_UnitBase::OnMapMoveCommand ( FVector TargetLocation )
{
	ProcessMoveCommand ( TargetLocation.X , TargetLocation.Y );
}

float AB_UnitBase::TakeDamage ( float DamageAmount , FDamageEvent const& DamageEvent , AController* EventInstigator , AActor* DamageCauser )
{
	float ActualDamage = Super::TakeDamage ( DamageAmount , DamageEvent , EventInstigator , DamageCauser );


	OnTakeDamage_Unit ( this , ActualDamage , nullptr , EventInstigator , DamageCauser );

	UE_LOG ( LogTemp , Warning , TEXT ( "%s에게 %.1f 데미지 / 남은 체력: %.1f" ) ,
		*DamageCauser->GetName () , ActualDamage , StatusComponent->CurrentHP );

	return ActualDamage;
}

void AB_UnitBase::UnitAttack(AActor* TargetActor)
{
	if (TargetActor == nullptr || !bIsAlive) return;

	FVector LookDir = TargetActor->GetActorLocation () - GetActorLocation ();
	LookDir.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX ( LookDir ).Rotator ();
	SetActorRotation ( TargetRot );

	UGameplayStatics::ApplyDamage (
		TargetActor,
		AttackDamage,    
		GetController(),
		this,
		UDamageType::StaticClass () // 데미지 유형
	);

	UE_LOG ( LogTemp , Warning , TEXT ( "% s 공격!" ) , *TargetActor->GetName () );


	FVector MuzzleLoc = GetActorLocation ();
	if (GetMesh ()->DoesSocketExist ( MuzzleSocketName ))
	{
		MuzzleLoc = GetMesh ()->GetSocketLocation ( MuzzleSocketName );
	}
	DrawDebugLine ( GetWorld () , MuzzleLoc , TargetActor->GetActorLocation () , FColor::Red , false , 0.2f , 0 , 1.0f );
}

void AB_UnitBase::OnTakeDamage_Unit ( AActor* DamagedActor , float Damage , const UDamageType* DamageType , AController* InstigateBy , AActor* DamageCauser )
{
	if (!bIsAlive) return;

	StatusComponent->TakeDamage(Damage);
	if (!IsAlive ())
	{
		OnDie_Unit ();
	}
}

void AB_UnitBase::OnDie_Unit ()
{
	bIsAlive = false;
	OnUnitDieDelegate.Broadcast ();
}
