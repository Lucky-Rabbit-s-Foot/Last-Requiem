// Fill out your copyright notice in the Description page of Project Settings.

#include "BJM/Unit/B_UnitBase.h"
#include "NavigationSystem.h"
#include "BJM/Unit/B_UnitStatusComponent.h"
#include "BJM/Unit/B_UnitAIController.h"
#include "AIController.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
#include "KWB/UI/W_MapWidget.h"
#include "Perception/AISense_Damage.h"
#include "TimerManager.h"

// Sets default values
AB_UnitBase::AB_UnitBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatusComponent = CreateDefaultSubobject<UB_UnitStatusComponent>(TEXT("StatusComponent"));

	AIControllerClass = AB_UnitAIController::StaticClass ();

	bUseControllerRotationYaw = false; 
	GetCharacterMovement()->bOrientRotationToMovement = true; 

	static FGameplayTag UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
	GameplayTags.AddTag ( UnitTag );

}

void AB_UnitBase::GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const
{
	TagContainer = GameplayTags;
}

// Called when the game starts or when spawned
void AB_UnitBase::BeginPlay()
{
	Super::BeginPlay();

	
	GetWorld ()->GetTimerManager ().SetTimer (
		FindWidgetTimerHandle ,
		this ,
		&AB_UnitBase::FindMapWidgetLoop ,
		0.5f ,
		true
	);

	UE_LOG ( LogTemp , Warning , TEXT ( "위젯 찾기 시작" ) );


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

void AB_UnitBase::FindMapWidgetLoop ()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass ( GetWorld () , FoundWidgets , UW_MapWidget::StaticClass () , false );

	if (FoundWidgets.Num () > 0)
	{
		UW_MapWidget* MapWidget = Cast<UW_MapWidget> ( FoundWidgets[0] );
		if (MapWidget)
		{
		
			MapWidget->OnRightMouseButtonClicked.AddDynamic ( this , &AB_UnitBase::CommandMoveToLocation );

			UE_LOG ( LogTemp , Warning , TEXT ( "연결 성공" ) );

			GetWorld ()->GetTimerManager ().ClearTimer ( FindWidgetTimerHandle );
		}
	}
	else
	{
		 UE_LOG(LogTemp, Log, TEXT("위젯 아직 못찾음"));
	}
}

FUnitProfileData AB_UnitBase::GetUnitProfileData ()
{
	FUnitProfileData Data;

	// 고정정보
	Data.ProfileImage = MyProfileImage;
	Data.UnitName = MyUnitName;

	// 변동정보
	if (StatusComponent)
	{
		Data.MaxHP = StatusComponent->MaxHP;
		Data.CurrentHP = StatusComponent->CurrentHP;

		Data.MaxSanity = StatusComponent->MaxSanity;
		Data.CurrentSanity = StatusComponent->CurrentSanity;

		Data.bIsInCombat = StatusComponent->bIsInCombat;
		Data.bIsSpeaking = StatusComponent->bIsSpeaking;
	}

	Data.bIsAlive = bIsAlive;

	return Data;
}

void AB_UnitBase::UnitMentalCheck_Move(float InX, float InY)
{

	if (StatusComponent == nullptr)
	{
		return;
	}

	// 현재 상태 확인
	EUnitBehaviorState CurrentState = StatusComponent->CurrentState;

	switch (CurrentState)
	{

		case EUnitBehaviorState::Awakened:
			ProcessMoveCommand(InX, InY);
			break;
		case EUnitBehaviorState::Normal:
			ProcessMoveCommand(InX, InY);
			break;
		case EUnitBehaviorState::Tense:
			// 50퍼 확률로 명령 수행
			if (FMath::RandRange(0, 100) > 50)
			{
				ProcessMoveCommand(InX, InY);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("나태"));
			}
			break;
		case EUnitBehaviorState::Fear:
			UE_LOG(LogTemp, Error, TEXT("공포"));
			break;
		case EUnitBehaviorState::Panic:
			UE_LOG(LogTemp, Error, TEXT("패닉"));
			break;
		case EUnitBehaviorState::Madness:
			UE_LOG(LogTemp, Error, TEXT("광기"));
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


void AB_UnitBase::CommandMoveToLocation ( FVector TargetLocation )
{

	DrawDebugSphere ( GetWorld () , TargetLocation , 50.0f , 16 , FColor::Red , false , 3.0f );
	UE_LOG ( LogTemp , Error , TEXT ( "이동좌표: %s" ) , *TargetLocation.ToString () );

	AAIController* AIController = Cast<AAIController> ( GetController () );
	if (!AIController)
	{
		UE_LOG ( LogTemp , Error , TEXT ( "AIController 없음" ) );
		return;
	}

	FVector SearchCenter = TargetLocation;
	SearchCenter.Z = GetActorLocation ().Z;

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1> ( GetWorld () );
	FNavLocation ProjectedLocation;

	FVector SearchExtent ( 500.0f , 500.0f , 5000.0f );

	FVector FinalLocation = TargetLocation; 

	if (NavSystem && NavSystem->ProjectPointToNavigation ( SearchCenter , ProjectedLocation , SearchExtent ))
	{
	
		FinalLocation = ProjectedLocation.Location;

		DrawDebugSphere ( GetWorld () , ProjectedLocation.Location , 50.0f , 16 , FColor::Green , false , 3.0f );
		UE_LOG ( LogTemp , Warning , TEXT ( "보정된 좌표: %s" ) , *ProjectedLocation.Location.ToString () );

		// 이동 명령
		//AIController->MoveToLocation ( ProjectedLocation.Location );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "네비 메시 못 찾음" ) );
	}

	UnitMentalCheck_Move ( FinalLocation.X , FinalLocation.Y );
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

	UAISense_Damage::ReportDamageEvent (
		GetWorld () ,
		TargetActor ,                     // 맞은 놈 (적 AI)
		this ,                            // 때린 놈 (플레이어)
		10.0f ,                           // 데미지 양
		this->GetActorLocation () ,       // 때린 위치
		TargetActor->GetActorLocation ()  // 맞은 위치
	);

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
