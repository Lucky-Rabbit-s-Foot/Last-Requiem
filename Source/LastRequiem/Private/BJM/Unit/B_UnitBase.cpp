// Fill out your copyright notice in the Description page of Project Settings.

#include "BJM/Unit/B_UnitBase.h"
#include "NavigationSystem.h"
#include "BJM/Unit/B_UnitStatusComponent.h"
#include "BJM/Unit/B_UnitAIController.h"
#include "BJM/Announcer/B_UIAnnouncer.h"
#include "PJB/Fortress/P_Fortress.h"
#include "AIController.h" 
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "DrawDebugHelpers.h"
#include "KWB/UI/Monitor/W_SituationMapWidget.h"
#include "KWB/UI/W_MapWidget.h"
#include "KWB/Component/IndicatorSpriteComponent.h"
#include "KWB/Component/W_SelectedSpriteComponent.h"
#include "Perception/AISense_Damage.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagsManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/AudioComponent.h"  
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundAttenuation.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/WidgetComponent.h"
#include "KWB/UI/W_ShowNameWidget.h"
#include "KHS/Drone/K_Drone.h"

// Sets default values
AB_UnitBase::AB_UnitBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent> ( TEXT ( "WeaponMesh" ) );
	WeaponMesh->SetupAttachment ( GetMesh () , FName ( "WeaponSocket" ) );
	WeaponMesh->SetCollisionProfileName ( TEXT ( "NoCollision" ) );

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect_V3"));
	NiagaraComp->SetupAttachment(WeaponMesh, FName("Muzzle"));
	NiagaraComp->bAutoActivate = false;

	GunFlashlight = CreateDefaultSubobject<USpotLightComponent> ( TEXT ( "GunFlashlight" ) );
	GunFlashlight->SetupAttachment ( WeaponMesh , FName ( "Flash" ) );
	GunFlashlight->SetIntensity ( 3000.0f );       // 밝기 
	GunFlashlight->SetOuterConeAngle ( 15.0f );    // 각도
	GunFlashlight->SetAttenuationRadius ( 1000.0f ); // 거리
	GunFlashlight->SetLightColor ( FLinearColor ( 1.0f , 0.95f , 0.8f ) );
	// 그림자
	GunFlashlight->SetCastShadows ( true );


	StatusComponent = CreateDefaultSubobject<UB_UnitStatusComponent>(TEXT("StatusComponent"));

	AIControllerClass = AB_UnitAIController::StaticClass ();

	bUseControllerRotationYaw = false; 
	GetCharacterMovement()->bOrientRotationToMovement = true; 

	//static FGameplayTag UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
	//GameplayTags.AddTag ( UnitTag );

	// 우빈님 추가
	IndicatorSprite = CreateDefaultSubobject<UIndicatorSpriteComponent> ( TEXT ( "IndicatorSprite" ) );
	IndicatorSprite->SetupAttachment ( RootComponent );

	ShowNameWidgetComponent = CreateDefaultSubobject<UWidgetComponent> ( TEXT ( "ShowNameWidget" ) );
	ShowNameWidgetComponent->SetupAttachment ( RootComponent );
	ShowNameWidgetComponent->SetWidgetSpace ( EWidgetSpace::Screen );
	ShowNameWidgetComponent->SetDrawAtDesiredSize ( true );
	ShowNameWidgetComponent->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	ShowNameWidgetComponent->SetRelativeLocation ( FVector ( 0.0f , 0.0f , 120.0f ) );

	SelectedSpriteComponent = CreateDefaultSubobject<UW_SelectedSpriteComponent> ( TEXT ( "SelectedSpriteComponent" ) );
	SelectedSpriteComponent->SetupAttachment ( RootComponent );

	SelectedSpriteComponent->SetVisibility ( false , true );
	SelectedSpriteComponent->SetHiddenInGame ( true );
	SelectedSpriteComponent->SetCollisionEnabled ( ECollisionEnabled::NoCollision );

}

void AB_UnitBase::GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const
{
	TagContainer = GameplayTags;
}

// Called when the game starts or when spawned
void AB_UnitBase::BeginPlay()
{
	Super::BeginPlay();

	//GetWorld ()->GetTimerManager ().SetTimer (
	//	FindWidgetTimerHandle ,
	//	this ,
	//	&AB_UnitBase::FindMapWidgetLoop ,
	//	0.5f ,
	//	true
	//);

	//UE_LOG ( LogTemp , Warning , TEXT ( "위젯 찾기 시작" ) );

	FGameplayTag UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
	GameplayTags.AddTag ( UnitTag );

	OriginalAttackDamage = AttackDamage;
	OriginalAttackRange = AttackRange;

	if (StatusComponent)
	{
		StatusComponent->OnStateChanged.AddDynamic ( this , &AB_UnitBase::OnBehaviorStateChanged_Unit );
		StatusComponent->OnHPChanged.AddDynamic ( this , &AB_UnitBase::OnHPChanged_Wrapper );
		StatusComponent->OnSanityChanged.AddDynamic ( this , &AB_UnitBase::OnSanityChanged_Wrapper );
		StatusComponent->OnCombatStateChanged.AddDynamic ( this , &AB_UnitBase::OnCombatStateChanged_Wrapper );
	}

	UnitDataUpdate ();
	InitializeShowNameWidget ();
	BindDroneDetection ();
}

void AB_UnitBase::EndPlay ( const EEndPlayReason::Type EndPlayReason )
{
	UnbindDroneDetection ();

	Super::EndPlay ( EndPlayReason );
}

// Called every frame
void AB_UnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UnitDataUpdate ();

}

// Called to bind functionality to input
void AB_UnitBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AB_UnitBase::InitializeShowNameWidget ()
{
	if (!ShowNameWidgetComponent)
	{
		return;
	}

	if (ShowNameWidgetClass)
	{
		ShowNameWidgetComponent->SetWidgetClass ( ShowNameWidgetClass );
	}

	ShowNameWidgetComponent->InitWidget ();
	ShowNameWidget = Cast<UW_ShowNameWidget> ( ShowNameWidgetComponent->GetUserWidgetObject () );
	if (ShowNameWidget)
	{
		ShowNameWidget->InitializeOwner ( this );
		ShowNameWidget->SetShowNameState ( EUnitShowNameState::UNDETECTED , false );
	}
}

void AB_UnitBase::BindDroneDetection ()
{
	if (BoundDrone.IsValid ())
	{
		return;
	}

	AK_Drone* Drone = Cast<AK_Drone> ( UGameplayStatics::GetPlayerPawn ( GetWorld () , 0 ) );
	if (!Drone)
	{
		return;
	}

	BoundDrone = Drone;
	UnitDetectedHandle = Drone->onUnitDetected.AddUObject ( this , &AB_UnitBase::HandleDroneUnitDetected );
	UnitLostHandle = Drone->onUnitLostDetection.AddUObject ( this , &AB_UnitBase::HandleDroneUnitLost );

	Drone->onUnitDetected.Broadcast ( this );
	
	Drone->onUnitLostDetection.Broadcast ( this );
}

void AB_UnitBase::UnbindDroneDetection ()
{
	if (!BoundDrone.IsValid ())
	{
		return;
	}

	if (UnitDetectedHandle.IsValid ())
	{
		BoundDrone->onUnitDetected.Remove ( UnitDetectedHandle );
	}

	if (UnitLostHandle.IsValid ())
	{
		BoundDrone->onUnitLostDetection.Remove ( UnitLostHandle );
	}

	BoundDrone.Reset ();
	UnitDetectedHandle.Reset ();
	UnitLostHandle.Reset ();
}

void AB_UnitBase::HandleDroneUnitDetected ( AActor* DetectedActor )
{
	if (DetectedActor != this || !ShowNameWidget)
	{
		return;
	}

	ShowNameWidget->SetShowNameState ( EUnitShowNameState::DETECTED , true );
}

void AB_UnitBase::HandleDroneUnitLost ( AActor* LostActor )
{
	if (LostActor != this || !ShowNameWidget)
	{
		return;
	}

	ShowNameWidget->SetShowNameState ( EUnitShowNameState::UNDETECTED , false );
}

//void AB_UnitBase::FindMapWidgetLoop ()
//{
//
//	TArray<UUserWidget*> FoundWidgets;
//
//	// 1. 가장 큰 부모인 SituationMapWidget을 찾는다.
//	UWidgetBlueprintLibrary::GetAllWidgetsOfClass ( GetWorld () , FoundWidgets , UW_SituationMapWidget::StaticClass () , false );
//
//	if (FoundWidgets.Num () > 0)
//	{
//		UW_SituationMapWidget* SituationWidget = Cast<UW_SituationMapWidget> ( FoundWidgets[0] );
//
//		if (SituationWidget)
//		{
//			SituationWidget->OnAttackButtonClicked.AddDynamic ( this , &AB_UnitBase::OnAttackButtonClicked_Unit );
//			SituationWidget->OnStopButtonClicked.AddDynamic ( this , &AB_UnitBase::OnStopButtonClicked_Unit );
//			SituationWidget->OnHoldButtonClicked.AddDynamic ( this , &AB_UnitBase::OnHoldButtonClicked_Unit );
//			SituationWidget->OnRetreatButtonClicked.AddDynamic ( this , &AB_UnitBase::OnRetreatButtonClicked_Unit );
//
//			UW_MapWidget* MapWidget = SituationWidget->GetRenderedMap ();
//			if (MapWidget)
//			{
//				MapWidget->OnRightMouseButtonClicked.AddDynamic ( this , &AB_UnitBase::CommandMoveToLocation );
//				UE_LOG ( LogTemp , Warning , TEXT ( "맵 이동 명령 연결 성공" ) );
//			}
//			else
//			{
//				UE_LOG ( LogTemp , Error , TEXT ( "SituationWidget은 찾았는데 그 안에 RenderedMap이 비어있음" ) );
//			}
//
//			UE_LOG ( LogTemp , Warning , TEXT ( "위젯 찾음 & 모든 연결 완료!" ) );
//
//			// 4. 타이머 해제
//			GetWorld ()->GetTimerManager ().ClearTimer ( FindWidgetTimerHandle );
//		} 
//	}
//	else
//	{
//		UE_LOG ( LogTemp , Log , TEXT ( "SituationMapWidget 아직 못 찾음..." ) );
//	}
//
//}


FUnitProfileData AB_UnitBase::GetUnitProfileData ()
{
	//FUnitProfileData Data;

	//// 고정정보
	//Data.ProfileImage = MyProfileImage;
	//Data.UnitName = MyUnitName;

	//// 변동정보
	//if (StatusComponent)
	//{
	//	Data.MaxHP = StatusComponent->MaxHP;
	//	Data.CurrentHP = StatusComponent->CurrentHP;

	//	Data.MaxSanity = StatusComponent->MaxSanity;
	//	Data.CurrentSanity = StatusComponent->CurrentSanity;

	//	Data.bIsInCombat = StatusComponent->bIsInCombat;
	//	Data.bIsSpeaking = StatusComponent->bIsSpeaking;
	//}

	//Data.bIsAlive = bIsAlive;

	return UnitUpdateData;
}

void AB_UnitBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction ( Transform );

	if (!UnitDataTable)
	{
		return;
	}

	const UEnum* UnitTypeEnum = StaticEnum<EUnitType> ();
	if (UnitTypeEnum)
	{
		FName RowName = FName ( *UnitTypeEnum->GetNameStringByValue ( (int64)UnitType ) );

		FUnitDataTableRow* UnitData = UnitDataTable->FindRow<FUnitDataTableRow> ( RowName , TEXT ( "UnitDataInit" ) );

		if (UnitData)
		{
			MyUnitName = UnitData->UnitName;
			MyProfileImage = UnitData->ProfileImage;

		}
	}
}

void AB_UnitBase::UnitDataUpdate ()
{
	UnitUpdateData.ProfileImage = MyProfileImage;
	UnitUpdateData.UnitName = MyUnitName;
	UnitUpdateData.CurrentHP = StatusComponent->CurrentHP;
	UnitUpdateData.MaxHP = StatusComponent->MaxHP;
	UnitUpdateData.CurrentSanity = StatusComponent->CurrentSanity;
	UnitUpdateData.MaxSanity = StatusComponent->MaxSanity;
	UnitUpdateData.bIsInCombat = StatusComponent->bIsInCombat;
	UnitUpdateData.bIsAlive = bIsAlive;
	UnitUpdateData.bIsSpeaking = bIsSpeaking;
}

void AB_UnitBase::UnitMentalCheck_Move(float InX, float InY)
{

	if (StatusComponent == nullptr)
	{
		return;
	}

	// 현재 상태 확인
	EUnitBehaviorState CurrentState = StatusComponent->CurrentState;

	bool bCanMove = false;

	switch (CurrentState)
	{

		case EUnitBehaviorState::Awakened:
		case EUnitBehaviorState::Normal:
			bCanMove = true;
			break;
		case EUnitBehaviorState::Tense:
			// 50퍼 확률로 명령 수행
			if (FMath::RandRange(0, 100) <= 80)
			{
				bCanMove = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("긴장"));
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
	if (bCanMove)
	{
		PlayCommandSound(Cmd_MoveSound);
		ProcessMoveCommand ( InX , InY );
	}
	else
	{
		PlayCommandSound(Cmd_DisobeySound);
	}

}

void AB_UnitBase::OnBehaviorStateChanged_Unit ( EUnitBehaviorState NewState )
{

	if (!bIsAlive) return;

	AttackDamage = OriginalAttackDamage;
	AttackRange = OriginalAttackRange;

	FGameplayTag UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
	FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag ( FName ( "Enemy" ) );

	GameplayTags.AddTag ( UnitTag );
	GameplayTags.RemoveTag ( EnemyTag );

	switch (NewState)
	{
	case EUnitBehaviorState::Awakened:
		AttackDamage = OriginalAttackDamage * 1.5f;
		AttackRange = OriginalAttackRange * 1.5f;
		UE_LOG ( LogTemp , Warning , TEXT ( "유닛 각성" ) );
		break;

	case EUnitBehaviorState::Fear:
		CommandRetreat ();
		UE_LOG ( LogTemp , Warning , TEXT ( "공포 / 요새로 후퇴" ) );
		break;

	case EUnitBehaviorState::Panic:
		CommandStop ();
		UE_LOG ( LogTemp , Warning , TEXT ( "패닉 상태! 통제 불능!" ) );
		break;

	case EUnitBehaviorState::Madness:
		//GameplayTags.RemoveTag ( UnitTag );
		//GameplayTags.AddTag ( EnemyTag );
		UE_LOG ( LogTemp , Warning , TEXT ( "광기 / 팀킬 시작" ) );
		break;
	}
	UnitDataUpdate ();
}

void AB_UnitBase::ProcessMoveCommand(float InX, float InY)
{
	AAIController* AIController = Cast<AAIController> ( GetController () );
	if (!AIController) return;

	if (StatusComponent)
	{
		EUnitBehaviorState State = StatusComponent->CurrentState;
		if (State == EUnitBehaviorState::Fear ||
			State == EUnitBehaviorState::Panic ||
			State == EUnitBehaviorState::Madness)
		{
			PlayCommandSound ( Cmd_DisobeySound );
			UE_LOG ( LogTemp , Warning , TEXT ( "상태 이상으로 명령 거부됨" ) );
			return;
		}
	}

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent ();
	if (BlackboardComp)
	{
		AIController->StopMovement ();

		BlackboardComp->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );

		// 1. 목표 좌표 입력
		FVector TargetLocation ( InX , InY , GetActorLocation ().Z );
		BlackboardComp->SetValueAsVector ( TEXT ( "TargetLocation" ) , TargetLocation );

		// 2. 공격 모드(A키)가 켜져있었다면 'AttackMove', 아니면 그냥 'Move'
		if (bIsAttackMode)
		{
			// 어택 땅 (이동하다 적 만나면 싸움)
			BlackboardComp->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::AttackMove ); // Enum에 AttackMove가 있어야 함! 없다면 Attack으로 대체
			UE_LOG ( LogTemp , Warning , TEXT ( "공격 이동" ) );

			// 명령 내렸으니 공격 모드 해제
			bIsAttackMode = false;
		}
		else
		{
			// 일반 이동
			BlackboardComp->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::Move );
			UE_LOG ( LogTemp , Warning , TEXT ( "일반 이동" ) );
		}
	}



	//// 현재 내 컨트롤러 가져오기
	//AController* controller = GetController();

	//// 내가 만든 AI 컨트롤러인지 확인
	//AB_UnitAIController* AIController = Cast<AB_UnitAIController>(controller);

	//if (AIController)
	//{
	//	// 목표 위치 만들기
	//	FVector TargetLocation(InX, InY, GetActorLocation().Z);

	//	// 목표 지점
	//	AIController->MoveToLocation(TargetLocation);

	//}
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

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent ();
	if (!BlackboardComp)
	{
		UE_LOG ( LogTemp , Error , TEXT ( "BlackboardComponent 없음" ) );
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
		BlackboardComp->SetValueAsVector ( TEXT ( "TargetLocation" ) , FinalLocation );
		BlackboardComp->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::Move );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "네비 메시 못 찾음" ) );
	}

	UnitMentalCheck_Move ( FinalLocation.X , FinalLocation.Y );
}

void AB_UnitBase::CommandAttackMove ( FVector TargetLocation )
{
	if (StatusComponent)
	{
		EUnitBehaviorState State = StatusComponent->CurrentState;
		if (State == EUnitBehaviorState::Fear || State == EUnitBehaviorState::Panic || State == EUnitBehaviorState::Madness)
		{
			PlayCommandSound ( Cmd_DisobeySound );
			return;
		}
	}

	PlayCommandSound(Cmd_AttackSound);

	AAIController* AIController = Cast<AAIController> ( GetController () );
	if (AIController && AIController->GetBlackboardComponent ())
	{
		// 1. 목표 좌표 설정
		AIController->GetBlackboardComponent ()->SetValueAsVector ( TEXT ( "TargetLocation" ) , TargetLocation );
		// 2. 상태를 AttackMove (없으면 Attack)로 변경
		AIController->GetBlackboardComponent ()->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::AttackMove );

	}
}

void AB_UnitBase::CommandStop ()
{
	if (StatusComponent)
	{
		EUnitBehaviorState State = StatusComponent->CurrentState;
		if (State == EUnitBehaviorState::Fear || State == EUnitBehaviorState::Panic || State == EUnitBehaviorState::Madness)
		{
			PlayCommandSound ( Cmd_DisobeySound );
			return; // 무시
		}
	}

	AAIController* AIController = Cast<AAIController> ( GetController () );
	if (AIController)
	{
		AIController->StopMovement (); // 즉시 정지

		if (AIController->GetBlackboardComponent ())
		{
			AIController->GetBlackboardComponent ()->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::Stop );
		}
	}
}

void AB_UnitBase::CommandHold ()
{
	if (StatusComponent)
	{
		EUnitBehaviorState State = StatusComponent->CurrentState;
		if (State == EUnitBehaviorState::Fear || State == EUnitBehaviorState::Panic || State == EUnitBehaviorState::Madness)
		{
			PlayCommandSound ( Cmd_DisobeySound );
			return; // 무시
		}
	}

	AAIController* AIController = Cast<AAIController> ( GetController () );
	if (AIController)
	{
		AIController->StopMovement (); // 일단 정지

		if (AIController->GetBlackboardComponent ())
		{
			AIController->GetBlackboardComponent ()->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::Hold );
		}
	}
}

void AB_UnitBase::CommandRetreat ()
{
	if (StatusComponent)
	{
		EUnitBehaviorState State = StatusComponent->CurrentState;
		if (State == EUnitBehaviorState::Fear || State == EUnitBehaviorState::Panic || State == EUnitBehaviorState::Madness)
		{
			PlayCommandSound ( Cmd_DisobeySound );
			return; // 무시
		}
	}

	AAIController* AIController = Cast<AAIController> ( GetController () );
	if (!AIController) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass ( GetWorld () , AP_Fortress::StaticClass () , FoundActors );

	FVector RetreatLocation = FVector::ZeroVector;
	bool bFoundFortress = false;

	FGameplayTag FortressTag = FGameplayTag::RequestGameplayTag ( FName ( "Fortress" ) );

	for (AActor* Actor : FoundActors)
	{
		AP_Fortress* Fortress = Cast<AP_Fortress> ( Actor );
		if (Fortress)
		{
			FGameplayTagContainer FortressTags;
			Fortress->GetOwnedGameplayTags ( FortressTags );

			if (FortressTags.HasTag ( FortressTag ))
			{
				RetreatLocation = Fortress->GetActorLocation ();
				bFoundFortress = true;
				UE_LOG ( LogTemp , Warning , TEXT ( "요새 발견: %s" ) , *Fortress->GetName () );
				break;
			}
		}
	}

	if (bFoundFortress && AIController->GetBlackboardComponent ())
	{

		AIController->GetBlackboardComponent ()->SetValueAsVector ( TEXT ( "TargetLocation" ) , RetreatLocation );

		AIController->GetBlackboardComponent ()->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::Retreat );

		AIController->GetBlackboardComponent ()->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
		AIController->ClearFocus ( EAIFocusPriority::Gameplay );

		UE_LOG ( LogTemp , Warning , TEXT ( "후퇴 시작" ) );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "요새 발견 x" ) );
	}


	//if (AIController && AIController->GetBlackboardComponent ())
	//{
	//	// 본진 좌표 입력
	//	AIController->GetBlackboardComponent ()->SetValueAsVector ( TEXT ( "TargetLocation" ) , FortressLocation );
	//	// 상태를 Retreat으로 변경
	//	AIController->GetBlackboardComponent ()->SetValueAsEnum ( TEXT ( "Command" ) , (uint8)EUnitCommandType::Retreat );

	//}
}

void AB_UnitBase::OnAttackButtonClicked_Unit ()
{
	bIsAttackMode = true;
	UE_LOG ( LogTemp , Warning , TEXT ( "공격모드" ));	
}

void AB_UnitBase::OnStopButtonClicked_Unit ()
{
	PlayCommandSound(Cmd_StopSound);
	bIsAttackMode = false;
	UE_LOG ( LogTemp , Warning , TEXT ( "스땁" ));
	CommandStop ();
}

void AB_UnitBase::OnHoldButtonClicked_Unit ()
{
	PlayCommandSound(Cmd_HoldSound);
	bIsAttackMode = false;
	UE_LOG ( LogTemp , Warning , TEXT ( "홀드" ));
	CommandHold ();
}

void AB_UnitBase::OnRetreatButtonClicked_Unit ()
{
	PlayCommandSound(Cmd_RetreatSound);
	bIsAttackMode = false;
	UE_LOG ( LogTemp , Warning , TEXT ( "후토ㅓㅣ" ));
	CommandRetreat ();
}

void AB_UnitBase::ReceiveSupport_HP ( float InValue )
{
	if (!StatusComponent || !bIsAlive) return;

	float RecoverHPAmount = InValue * HPRecoveryRatio;
	StatusComponent->RecoverHP ( RecoverHPAmount );
	PlayVoiceForEvent(EUnitVoiceEvent::Recovery);
	PlayHealingHPEffect();
}

void AB_UnitBase::ReceiveSupport_Sanity ( float InValue )
{

	if (!StatusComponent || !bIsAlive) return;
	float RecoverSanityAmount = InValue * SanityRecoveryRatio;
	StatusComponent->RecoverSanity ( RecoverSanityAmount );
	PlayVoiceForEvent(EUnitVoiceEvent::Recovery);
	PlayHealingSanityEffect();
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

	if (!TargetActor || !bIsAlive) return;

	IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface> ( TargetActor );
	if (TagInterface)
	{
		FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag ( FName ( "Enemy" ) );
		FGameplayTagContainer TargetTags;
		TagInterface->GetOwnedGameplayTags ( TargetTags );

		bool bIsTargetEnemy = TargetTags.HasTag ( EnemyTag );
		bool bAmIMad = (StatusComponent && StatusComponent->CurrentState == EUnitBehaviorState::Madness);


		//if (!TargetTags.HasTag ( EnemyTag ))
		//{
		//	CurrentAttackTarget = nullptr;

		//	AAIController* AIC = Cast<AAIController> ( GetController () );
		//	if (AIC && AIC->GetBlackboardComponent ())
		//	{
		//		AIC->GetBlackboardComponent ()->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
		//	}
		//	return;
		//}

		if (!bIsTargetEnemy && !bAmIMad)
		{
			CurrentAttackTarget = nullptr;

			AAIController* AIC = Cast<AAIController> ( GetController () );
			if (AIC && AIC->GetBlackboardComponent ())
			{
				AIC->GetBlackboardComponent ()->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
			}
			return;
		}

	}

	SetCombatState_Unit ( true );

	CurrentAttackTarget = TargetActor;

	FVector LookDir = TargetActor->GetActorLocation () - GetActorLocation ();
	LookDir.Z = 0.0f;
	SetActorRotation ( FRotationMatrix::MakeFromX ( LookDir ).Rotator () );

	UAnimInstance* AnimInstance = GetMesh ()->GetAnimInstance ();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play ( AttackMontage );
		PlayVoiceForEvent(EUnitVoiceEvent::Combat);

		PlayMuzzleEffect();

	}
	else
	{
		OnAttackHit_Unit ();
		PlayVoiceForEvent(EUnitVoiceEvent::Combat);
	}
}

void AB_UnitBase::OnAttackHit_Unit ()
{
	if (!CurrentAttackTarget || !bIsAlive) return;

	float Distance = FVector::Dist ( GetActorLocation () , CurrentAttackTarget->GetActorLocation () );
	if (Distance > AttackRange + 100.0f) return; 

	UGameplayStatics::ApplyDamage (
		CurrentAttackTarget ,
		AttackDamage ,
		GetController () ,
		this ,
		UDamageType::StaticClass ()
	);

	FVector MuzzleLoc = GetActorLocation ();
	if (GetMesh ()->DoesSocketExist ( MuzzleSocketName ))
	{
		MuzzleLoc = GetMesh ()->GetSocketLocation ( MuzzleSocketName );
	}

	UE_LOG ( LogTemp , Warning , TEXT ( "타격" ) );
	PlayMuzzleEffect();


}

void AB_UnitBase::SetCombatState_Unit ( bool bInCombat )
{
	if (!StatusComponent) return;

	StatusComponent->SetCombatState ( bInCombat );

	if (StatusComponent->bIsInCombat != bInCombat)
	{
		StatusComponent->bIsInCombat = bInCombat;

		UnitDataUpdate ();
		OnCombatStateChanged.Broadcast ( bInCombat );

		if (bInCombat)
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "[%s] 전투 돌입" ) , *GetName () );
		}
		else
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "[%s] 전투 해제" ) , *GetName () );
		}
	}

	// 우빈님 추가
	if (IndicatorSprite)
	{
		IndicatorSprite->SetIndicatorState (
			bInCombat ? EIndicatorSpriteState::Combat : EIndicatorSpriteState::Normal );

		if (bInCombat)
		{
			IndicatorSprite->StartGlow (); // 무한
		}
		else
		{
			IndicatorSprite->StopGlow ();
		}
	}

}

void AB_UnitBase::OnTakeDamage_Unit ( AActor* DamagedActor , float Damage , const UDamageType* DamageType , AController* InstigateBy , AActor* DamageCauser )
{
	if (!bIsAlive) return;

	if (StatusComponent)
	{
		StatusComponent->ReduceHP ( Damage );

		UnitDataUpdate ();

		if (StatusComponent->CurrentHP <= 0.0f)
		{
			OnDie_Unit ();
		}
	}
}

void AB_UnitBase::OnDie_Unit ()
{
	if (!bIsAlive)
	{
		return;
	}

	PlayVoiceForEvent(EUnitVoiceEvent::Death);
	bIsAlive = false;

	// 택 제거
	FGameplayTag UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
	GameplayTags.RemoveTag ( UnitTag );

	// 캡슐 콜리전 해제
	GetCapsuleComponent ()->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	GetCapsuleComponent ()->SetCollisionResponseToAllChannels ( ECR_Ignore );

	if (UAnimInstance* AnimInstance = GetMesh ()->GetAnimInstance ())
	{
		AnimInstance->StopAllMontages ( 0.2f );
	}

	CurrentAttackTarget = nullptr;

	// 우빈님 추가
	if (IndicatorSprite)
	{
		bIsSelectedByPlayer = false;  
		RefreshIndicatorState();
	}


	float ShockRadius = 1500.0f;
	float DeathPenalty = -10.0f; 

	TArray<FOverlapResult> OverlapResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere ( ShockRadius );

	bool bResult = GetWorld ()->OverlapMultiByChannel (
		OverlapResults ,
		GetActorLocation () ,
		FQuat::Identity ,
		ECollisionChannel::ECC_Pawn ,
		CollisionShape
	);

	if (bResult)
	{
		DrawDebugSphere ( GetWorld () , GetActorLocation () , ShockRadius , 24 , FColor::Red , false , 2.0f );

		for (auto const& Overlap : OverlapResults)
		{
			AActor* OverlapActor = Overlap.GetActor ();

			// 나 제외
			if (OverlapActor == this) continue;

			// 아군 확인1
			AB_UnitBase* FellowUnit = Cast<AB_UnitBase> ( OverlapActor );
			if (FellowUnit && FellowUnit->IsAlive ())
			{
				// 아군확인2
				//FGameplayTag UnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
				if (FellowUnit->GameplayTags.HasTag ( UnitTag ))
				{
					if (FellowUnit->StatusComponent)
					{
						FellowUnit->StatusComponent->ModifySanity ( DeathPenalty );
						UE_LOG ( LogTemp , Warning , TEXT ( "%s가 %s의 죽음 목격" ) , *FellowUnit->GetName () , *GetName () );		}
				}
			}
		}
	}


	OnUnitDieDelegate.Broadcast ( this );

	if (AB_UIAnnouncer* Announcer = Cast<AB_UIAnnouncer> ( UGameplayStatics::GetActorOfClass ( GetWorld () , AB_UIAnnouncer::StaticClass () ) ))
	{
		Announcer->PlayUnitDeathSound ( MyUnitName.ToString () );
	}


	if (AAIController* AIController = Cast<AAIController> ( GetController () ))
	{
		if (UBlackboardComponent* BBComp = AIController->GetBlackboardComponent ())
		{
			BBComp->SetValueAsObject ( TEXT ( "TargetEnemy" ) , nullptr );
			BBComp->SetValueAsVector ( TEXT ( "TargetLocation" ) , GetActorLocation () ); // 이동 목표를 현재 위치로
		}

		if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent> ( AIController->GetBrainComponent () ))
		{
			BTComp->StopTree ( EBTStopMode::Safe );
		}

		AIController->StopMovement ();

		AIController->UnPossess ();

		AIController->Destroy ();
	}


	if (GetCharacterMovement ())
	{
		GetCharacterMovement ()->StopMovementImmediately ();
		GetCharacterMovement ()->DisableMovement ();
		GetCharacterMovement ()->SetComponentTickEnabled ( false ); // 틱 업데이트 중지
	}


	// SetLifeSpan ( 5.0f ); // 시체 5초 뒤 삭제

	UE_LOG ( LogTemp , Warning , TEXT ( "%s 사망 처리 완료" ) , *GetName () );

	//// 이동 뺌
	//if (AAIController* AIController = Cast<AAIController> ( GetController () ))
	//{
	//	AIController->StopMovement ();
	//	AIController->UnPossess ();
	//}

	//GetCapsuleComponent ()->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	//GetCapsuleComponent ()->SetCollisionResponseToAllChannels ( ECR_Ignore );

	////GetMesh ()->SetCollisionProfileName ( TEXT ( "Ragdoll" ) );
	////GetMesh ()->SetSimulatePhysics ( true );

	////SetLifeSpan ( 5.0f );

	//UE_LOG ( LogTemp , Warning , TEXT ( "%s 태그 제거" ) , *GetName () );
}

void AB_UnitBase::OnHPChanged_Wrapper ( float InCurrentHP , float InMaxHP )
{
	UnitDataUpdate ();
}

void AB_UnitBase::OnSanityChanged_Wrapper ( float InCurrentSanity , float InMaxSanity )
{
	UnitDataUpdate ();
}

void AB_UnitBase::OnCombatStateChanged_Wrapper ( bool bInCombat )
{
	RefreshIndicatorState();
	UnitDataUpdate();
}

void AB_UnitBase::SetSelectedSprite ( bool bIsSelected )
{
	if (!bIsAlive)
	{
		bIsSelectedByPlayer = false;
		RefreshIndicatorState();
		return;
	}

	bIsSelectedByPlayer = bIsSelected;
	RefreshIndicatorState(); 
}

void AB_UnitBase::SetSpeakingState ( bool bNewState )
{
	if (bIsSpeaking == bNewState) return;

	bIsSpeaking = bNewState;

	if (OnUnitSpeakDelegate.IsBound ())
	{
		OnUnitSpeakDelegate.Broadcast ( this , bIsSpeaking );
	}
	UnitDataUpdate ();
	
}

void AB_UnitBase::PlayUnitVoice ( USoundBase* InVoiceSound , FString StateText )
{

	if (!bIsAlive || InVoiceSound == nullptr) return;

	// 1. 기존에 재생 중이던 소리가 있다면 정리
	if (CurrentVoiceComp)
	{
		CurrentVoiceComp->Stop ();
		CurrentVoiceComp = nullptr;
	}

	// 2. 소리 컴포넌트 생성
	CurrentVoiceComp = UGameplayStatics::SpawnSoundAttached (
		InVoiceSound ,
		GetMesh () ,
		FName ( "HeadSocket" ) ,
		FVector::ZeroVector ,
		FRotator::ZeroRotator ,
		EAttachLocation::KeepRelativeOffset ,
		false ,
		1.0f ,
		1.0f ,
		0.0f ,
		UnitVoiceAttenuation ,
		UnitVoiceConcurrency
	);

	if (CurrentVoiceComp)
	{
		CurrentVoiceComp->OnAudioFinished.AddDynamic ( this , &AB_UnitBase::OnVoiceFinished );

		if (UnitSoundClass)
		{
			CurrentVoiceComp->SoundClassOverride = UnitSoundClass;
		}

		// 3. 일단 재생 요청!
		CurrentVoiceComp->Play ();

		// (패키징 빌드에서 동시성 처리가 너무 빨라서 생기는 문제 해결용)

		GetWorld ()->GetTimerManager ().SetTimer (
			TimerHandle_PlaybackCheck ,
			[this , StateText]() // 람다 함수 (즉석 함수)
			{
				// 0.05초 뒤에 실행되는 내용:
				if (CurrentVoiceComp && CurrentVoiceComp->IsPlaying ())
				{
					// 합격! 오디오 엔진이 허락했음 -> 말풍선 띄우기
					if (OnUnitSpeakChanged.IsBound ())
					{
						OnUnitSpeakChanged.Broadcast ( this , true , StateText );
					}
					SetSpeakingState ( true );

					UE_LOG ( LogTemp , Log , TEXT ( "[%s] 말하기 성공! (검증 완료)" ) , *GetName () );
				}
				else
				{
					// 불합격! 동시성 규칙 때문에 소리가 짤렸음 -> 말풍선 띄우지 마!
					SetSpeakingState ( false );
					UE_LOG ( LogTemp , Warning , TEXT ( "[%s] 말하기 취소됨 (동시성 제한)" ) , *GetName () );
				}
			} ,
			0.05f , // 0.05초 딜레이
			false
		);
	}
	else
	{
		SetSpeakingState ( false );
	}




	//if (!bIsAlive || InVoiceSound == nullptr) return;

	//// 2. 기존에 재생 중이던 소리가 있다면 강제 종료 (새 소리를 위해)
	//if (CurrentVoiceComp)
	//{
	//	CurrentVoiceComp->Stop (); // 이걸 호출하면 OnAudioFinished가 발동됨 -> 자막 꺼짐
	//	CurrentVoiceComp = nullptr;
	//}

	//// 3. 소리 재생 (컴포넌트 생성)
	//CurrentVoiceComp = UGameplayStatics::SpawnSoundAttached (
	//	InVoiceSound ,                        // 소리 파일
	//	GetMesh () ,                           // 붙일 곳
	//	FName ( "HeadSocket" ) ,                 // 소켓 이름
	//	FVector::ZeroVector ,                 // 위치 오프셋
	//	FRotator::ZeroRotator ,               // 회전값
	//	EAttachLocation::KeepRelativeOffset , // 붙이기 설정
	//	false ,                               // 액터 파괴 시 멈춤 여부
	//	1.0f ,                                // 볼륨 Multiplier
	//	1.0f ,                                // 피치 Multiplier
	//	0.0f ,                                // 시작 시간
	//	UnitVoiceAttenuation ,                // 감쇠 설정
	//	UnitVoiceConcurrency                 // ★ 동시성 설정 (여기서 2명 제한 작동!)
	//);

	//if (CurrentVoiceComp)
	//{
	//	CurrentVoiceComp->OnAudioFinished.AddDynamic ( this , &AB_UnitBase::OnVoiceFinished );

	//	if (UnitSoundClass)
	//	{
	//		CurrentVoiceComp->SoundClassOverride = UnitSoundClass;
	//	}

	//	CurrentVoiceComp->Play ();

	//	if (CurrentVoiceComp->IsPlaying ())
	//	{
	//		// 성공: 진짜 재생 중이니까 말풍선 띄워!
	//		if (OnUnitSpeakChanged.IsBound ())
	//		{
	//			OnUnitSpeakChanged.Broadcast ( this , true , StateText );
	//		}
	//		SetSpeakingState ( true );

	//		UE_LOG ( LogTemp , Log , TEXT ( "[%s] 말하기 성공! (상태: %s)" ) , *GetName () , *StateText );
	//	}
	//	else
	//	{
	//		// 실패: 동시성(Prevent New) 때문에 재생 즉시 차단됨
	//		// 말풍선 띄우지 말고 조용히 종료 처리
	//		UE_LOG ( LogTemp , Warning , TEXT ( "[%s] 말하기 실패 (동시성 제한 걸림)" ) , *GetName () );

	//		CurrentVoiceComp->Stop (); // 확실하게 멈춤
	//		CurrentVoiceComp = nullptr; // 포인터 비움
	//		SetSpeakingState ( false );
	//	}
	//}
	//else
	//{
	//	// 컨커런시 설정 때문에 소리 재생에 실패했다면 말하기 상태도 켜지 마라
	//	SetSpeakingState ( false );
	//}



	//UE_LOG ( LogTemp , Log , TEXT ( "[%s] 음성 재생 시작 (상태: %s)" ) , *GetName () , *StateText );








	//if (!bIsAlive || InVoiceSound == nullptr) return;



	//UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAttached(
	//	InVoiceSound,                        // 소리 파일
	//	GetMesh(),                           // 붙일 곳
	//	FName("HeadSocket"),                 // 소켓 이름
	//	FVector::ZeroVector,                 // 위치 오프셋
	//	FRotator::ZeroRotator,               // 회전값
	//	EAttachLocation::KeepRelativeOffset, // 붙이기 설정
	//	false,                               // 액터 파괴 시 멈춤 여부
	//	1.0f,                                // 볼륨 Multiplier
	//	1.0f,                                // 피치 Multiplier
	//	0.0f,                                // 시작 시간
	//	UnitVoiceAttenuation,                // 감쇠 설정
	//	UnitVoiceConcurrency				 // 동시성 설정
	//);

	//if (AudioComp)
	//{
	//	if (UnitSoundClass)
	//	{
	//		AudioComp->SoundClassOverride = UnitSoundClass;
	//	}

	//	AudioComp->Play();
	//}

	//SetSpeakingState ( true );

	//float SoundDuration = InVoiceSound->GetDuration ();

	//if (SoundDuration <= 0.0f) SoundDuration = 1.0f;

	//GetWorld ()->GetTimerManager ().ClearTimer ( SpeakingTimerHandle );

	//GetWorld ()->GetTimerManager ().SetTimer (
	//	SpeakingTimerHandle ,
	//	this ,
	//	&AB_UnitBase::StopUnitVoice , 
	//	SoundDuration ,
	//	false
	//);

	//UE_LOG ( LogTemp , Log , TEXT ( "[%s] 음성 재생 시작 (%.1f초)" ) , *GetName () , SoundDuration );
}

void AB_UnitBase::StopUnitVoice ()
{
	if (CurrentVoiceComp)
	{
		CurrentVoiceComp->Stop (); // 이걸 부르면 알아서 OnVoiceFinished로 감
	}
}

void AB_UnitBase::PlayVoiceForEvent(EUnitVoiceEvent InEvent)
{
	if (!StatusComponent || !bIsAlive) return;

	if (bIsSpeaking)
	{
		if (InEvent == EUnitVoiceEvent::Recovery || InEvent == EUnitVoiceEvent::Combat)
		{
			return;
		}

	}

	EUnitBehaviorState CurrentMentalState = StatusComponent->CurrentState;
	if (!UnitVoiceData.Contains(CurrentMentalState)) return;

	FUnitVoiceProfile VoiceProfile = UnitVoiceData[CurrentMentalState];
	USoundBase* SoundToPlay = nullptr;

	switch (InEvent)
	{
	case EUnitVoiceEvent::SpotEnemy:
		SoundToPlay = VoiceProfile.SpotEnemySound;

		bCanPlayCombatVoice = false;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_CombatVoiceCooldown,
			this,
			&AB_UnitBase::ResetCombatVoiceCooldown,
			5.0f, 
			false
		);
		break;

	case EUnitVoiceEvent::Combat:
		if (bCanPlayCombatVoice)
		{
			SoundToPlay = VoiceProfile.AttackSound;

			bCanPlayCombatVoice = false;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle_CombatVoiceCooldown,
				this,
				&AB_UnitBase::ResetCombatVoiceCooldown,
				7.0f,
				false
			);
		}
		break;

	case EUnitVoiceEvent::Recovery:
		SoundToPlay = VoiceProfile.RecoverySound;
		break;

	case EUnitVoiceEvent::Death:
		SoundToPlay = VoiceProfile.DeathSound;
		break;
	}

	if (SoundToPlay)
	{
		// 1. 현재 상태를 글자(String)로 변환
		FString StateText = GetMentalStateText ( CurrentMentalState );

		// 2. 소리와 함께 글자를 넘겨줌!
		PlayUnitVoice ( SoundToPlay , StateText );
	}
}

void AB_UnitBase::ResetCombatVoiceCooldown()
{
	bCanPlayCombatVoice = true;
}

void AB_UnitBase::PlayCommandSound(USoundBase* InSound)
{
	if (!InSound || !bIsAlive) return;

	UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D (
		GetWorld () ,
		InSound ,
		1.0f ,  // Volume
		1.0f ,  // Pitch
		0.0f ,  // StartTime
		UnitVoiceConcurrency
	);


	if (AudioComp)
	{
		if (UnitCommandSoundClass)
		{
			AudioComp->SoundClassOverride = UnitCommandSoundClass;
		}

		// 3. 재생!
		AudioComp->Play();
	}
}

void AB_UnitBase::PlayMuzzleEffect()
{
	if (MuzzleFlashVFX == nullptr || WeaponMesh == nullptr) return;

	if (NiagaraComp->GetAsset() != MuzzleFlashVFX)
	{
		NiagaraComp->SetAsset(MuzzleFlashVFX);
	}


	NiagaraComp->Activate(true);

	UE_LOG(LogTemp, Warning, TEXT("이펙트 발동! 위치: %s"), *NiagaraComp->GetComponentLocation().ToString());

	//UNiagaraFunctionLibrary::SpawnSystemAttached(
	//	MuzzleFlashVFX,
	//	WeaponMesh,
	//	FName("Muzzle"),
	//	FVector::ZeroVector,
	//	FRotator::ZeroRotator,
	//	EAttachLocation::KeepRelativeOffset,
	//	true
	//);
}

void AB_UnitBase::PlayFootstepSound()
{
	if (FootstepSound == nullptr) return;

	// 1. 소리 재생 (내 발 밑에서)
	UGameplayStatics::SpawnSoundAtLocation(
		this,
		FootstepSound,
		GetActorLocation() - FVector(0, 0, 90.0f),
		FRotator::ZeroRotator,
		1.0f, 
		1.0f, 
		0.0f,
		UnitVoiceAttenuation 
	);
}

void AB_UnitBase::PlayHealingHPEffect()
{
	if (HealingHP == nullptr) return;

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		HealingHP,                 
		GetMesh(),                 
		FName("root"),            
		FVector::ZeroVector,        
		FRotator::ZeroRotator,     
		EAttachLocation::SnapToTarget, 
		true                      
	);
}

void AB_UnitBase::PlayHealingSanityEffect()
{
	if (HealingSanity == nullptr) return;

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		HealingSanity,
		GetMesh(),
		FName("root"),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true
	);
}

void AB_UnitBase::RefreshIndicatorState()
{
	if (!IndicatorSprite)
		return;

	// 1) Dead가 최우선
	if (!bIsAlive)
	{
		IndicatorSprite->SetIndicatorState(EIndicatorSpriteState::Dead);

		IndicatorSprite->SetWorldRotation ( FRotator ( 0.0f , 90.0f , -90.0f ) );
		return;
	}

	// 2) Combat (StatusComponent 기준)
	const bool bInCombat = (StatusComponent && StatusComponent->bIsInCombat);
	if (bInCombat)
	{
		IndicatorSprite->SetIndicatorState(EIndicatorSpriteState::Combat);
		return;
	}

	// 3) Selected
	if (bIsSelectedByPlayer)
	{
		IndicatorSprite->SetIndicatorState(EIndicatorSpriteState::Selected);
		return;
	}

	// 4) Normal
	IndicatorSprite->SetIndicatorState(EIndicatorSpriteState::Normal);
}

void AB_UnitBase::OnVoiceFinished ()
{
	SetSpeakingState ( false );

	if (OnUnitSpeakChanged.IsBound ())
	{
		OnUnitSpeakChanged.Broadcast ( this , false , TEXT ( "" ) );
	}

	CurrentVoiceComp = nullptr;
}

FString AB_UnitBase::GetMentalStateText ( EUnitBehaviorState State )
{
	switch (State)
	{
	case EUnitBehaviorState::Normal:   return TEXT ( "NORMAL" );
	case EUnitBehaviorState::Tense:    return TEXT ( "TENSE" );
	case EUnitBehaviorState::Fear:     return TEXT ( "FEAR" );
	case EUnitBehaviorState::Panic:    return TEXT ( "PANIC!" );
	case EUnitBehaviorState::Madness:  return TEXT ( "MADNESS" );
	case EUnitBehaviorState::Awakened: return TEXT ( "AWAKENED" );
	default: return TEXT ( "" );
	}
}

void AB_UnitBase::PlayWeaponFireSound ()
{
	if (!WeaponFireSound) return;

	UGameplayStatics::SpawnSoundAttached (
		WeaponFireSound ,
		GetMesh () ,
		FName ( "Muzzle" ) ,
		FVector::ZeroVector ,
		FRotator::ZeroRotator ,
		EAttachLocation::KeepRelativeOffset ,
		true , // bStopWhenAttachedToDestroyed
		1.0f , // Volume
		1.0f , // Pitch
		0.0f , // StartTime
		nullptr // Attenuation
	);


}

