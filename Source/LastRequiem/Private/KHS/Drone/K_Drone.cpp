// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/Drone/K_Drone.h"
#include "KHS/Data/K_DroneData.h"

#include "BJM/Unit/B_UnitBase.h"
#include "BJM/Unit/B_UnitMentalTypes.h"

#include "KWB/Component/IndicatorSpriteComponent.h"
#include "TimerManager.h" // (20251223) W : 드론 스킬 쿨타임 용도

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundBase.h"
#include "KHS/Util/K_LoggingSystem.h"
#include "PJB/Enemy/P_EnemyBase.h"
#include "PJB/Obstacle/P_EnemyObstacle.h"


// Sets default values
AK_Drone::AK_Drone()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitializeDefaultComponents();
	
	bUseControllerRotationYaw = true;
	
	SetReplicates(true);
	SetReplicateMovement(true);
}

void AK_Drone::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	if (sphereComp)
	{
		sphereComp->SetSimulatePhysics(true);
		sphereComp->SetMassOverrideInKg(NAME_None, DRONE_MASS_WEIGHT); //드론 질량
	}
}

// Called when the game starts or when spawned
void AK_Drone::BeginPlay()
{
	Super::BeginPlay();
	//Anim
	UAnimInstance* animInst = meshComp->GetAnimInstance();
	if (flightMontage && animInst)
	{
		animInst->Montage_Play(flightMontage);
		animInst->Montage_JumpToSection(FName("Start"));
	}
	
	// (20251224) P : 드론 충돌 (Start)
	if(UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		DefaultLinearDamping = primComp->GetLinearDamping ();
	}
	// (20251224) P : 드론 충돌 (End)

	//Unit 최초 탐색 후
	//타이머로 n초마다 UpdateDetectedUnitSlot 호출
	InitializeDetectedUnitSlot();
	
	GetWorldTimerManager().SetTimer(
		detectionTimerHandle,
		this,
		&AK_Drone::UpdateDetectedUnitSlot,
		1.0f,
		true
	);

	if (audioComp && flightSound)
	{
		audioComp->SetSound ( flightSound );
		audioComp->Play ();
	}
}

void AK_Drone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (audioComp && audioComp->IsPlaying ())
	{
		audioComp->Stop ();
	}

	GetWorldTimerManager().ClearTimer(detectionTimerHandle);
	// (20251223) W : 드론 스킬 타이머 정리
	GetWorldTimerManager ().ClearTimer ( Skill01CooldownTimerHandle );
	GetWorldTimerManager ().ClearTimer ( Skill02CooldownTimerHandle );
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AK_Drone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDroneMovement(DeltaTime);
	UpdateDroneRotation(DeltaTime);

	if (audioComp && audioComp->IsPlaying ())
	{
		float velocitySize = GetVelocity ().Size ();
		float newPitch = FMath::GetMappedRangeValueClamped ( FVector2D ( 0.0f , 2000.0f ) , FVector2D ( 0.5f , 1.0f ) , velocitySize );
		audioComp->SetPitchMultiplier ( newPitch );
	}
}

// (20251224) P : 드론 충돌 (Start)
void AK_Drone::NotifyHit ( UPrimitiveComponent* MyComp , AActor* Other , UPrimitiveComponent* OtherComp , bool bSelfMoved , FVector HitLocation , FVector HitNormal , FVector NormalImpulse , const FHitResult& Hit )
{
	Super::NotifyHit ( MyComp , Other , OtherComp , bSelfMoved , HitLocation , HitNormal , NormalImpulse , Hit );

	if (Other && Cast<ACharacter> ( Other ))
	{
		if(GetWorldTimerManager ().IsTimerActive ( DampingRestoreTimer ))
		{
			return;
		}
		MyComp->SetLinearDamping ( 10.0f );
		GetWorldTimerManager ().SetTimer ( 
			DampingRestoreTimer , 
			FTimerDelegate::CreateLambda ( 
				[ this , MyComp ]() { MyComp->SetLinearDamping ( DefaultLinearDamping ); } 
			) ,
			0.2f , 
			false 
		);
	}
}
// (20251224) P : 드론 충돌 (End)

void AK_Drone::InitializeDefaultComponents()
{
	//sphere 
	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(100.f);
	
	//sphere 물리설정 추가
	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	sphereComp->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	sphereComp->SetCollisionResponseToAllChannels(ECR_Block);
	sphereComp->SetEnableGravity(false); 
	sphereComp->SetLinearDamping(1.2f);  //관성 감쇠
	sphereComp->SetAngularDamping(2.0f); //회전 감쇠
	
	//springArm
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	springArmComp->TargetArmLength = 300.f;
	springArmComp->bUsePawnControlRotation = true;
	
	//camera
	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(springArmComp);
	cameraComp->bUsePawnControlRotation = false;
	
	//mesh
	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>("meshComp");
	meshComp->SetupAttachment(RootComponent);
	
	//indicator
	indicatorComp = CreateDefaultSubobject<UIndicatorSpriteComponent>("indicatorComp");
	indicatorComp->SetupAttachment(RootComponent);

	//audio
	audioComp = CreateDefaultSubobject<UAudioComponent> ( "audioComp" );
	audioComp->SetupAttachment ( RootComponent );
	audioComp->bAutoActivate = false;
}

void AK_Drone::UpdateDroneMovement(float DeltaTime)
{
	if (!sphereComp)
	{
		return;
	}

	// (20251224) P : 드론 이동 (Start)
	FVector CurrentVelocity = sphereComp->GetPhysicsLinearVelocity ();
	bool bIsOverLimitHeight = GetActorLocation ().Z > droneData->DRONE_FLIGHT_MAX_HEIGHT && CurrentVelocity.Z > 0.0f;
	bool bIsUnderLimitHeight = GetActorLocation ().Z < droneData->DRONE_FLIGHT_MIN_HEIGHT && CurrentVelocity.Z < 0.0f;
	if (bIsOverLimitHeight || bIsUnderLimitHeight)
	{
		CurrentVelocity.Z = 0.0f;
		sphereComp->SetPhysicsLinearVelocity ( CurrentVelocity );
	}
	// (20251224) P : 드론 이동 (End)

	//입력이 있을때만 Force 적용
	bool bHasInput = (moveInputValue.SizeSquared() > 0.0f || FMath::Abs(upDownInputValue) > 0.0f);
	
	if(bHasInput)
	{
		FVector forwardDir = GetActorForwardVector();
		FVector rightDir = GetActorRightVector();
		FVector upDir = FVector::UpVector;
		
		//방향별 Force 계산
		FVector horizontalForce = (forwardDir * moveInputValue.Y + rightDir * moveInputValue.X) * droneData->DRONE_HORIZONTAL_FORCE;
		FVector verticalForce = upDir * upDownInputValue * droneData->DRONE_VERTICAL_FORCE;
		
		//고도 제한 적용
		bool bOnLimitHeight = GetActorLocation().Z > droneData->DRONE_FLIGHT_MAX_HEIGHT && upDownInputValue > 0.0f;
		bool bOnLimitLowHeight = GetActorLocation ().Z < droneData->DRONE_FLIGHT_MIN_HEIGHT && upDownInputValue < 0.0f;
		if (bOnLimitHeight || bOnLimitLowHeight)
		{
			verticalForce = FVector::ZeroVector;
		}
		
		FVector totalForce = horizontalForce + verticalForce;
		
		sphereComp->AddForce(totalForce, NAME_None, true);
	}
}

void AK_Drone::UpdateDroneRotation(float DeltaTime)
{
	if (!GetController())
	{
		return;
	}
	
	FRotator currentRot = GetActorRotation();
	FRotator targetRot = GetController()->GetControlRotation();
	
	targetRot.Pitch = 0.0f;
	targetRot.Roll = 0.0f;
	
	FRotator newRot = FMath::RInterpTo(targetRot, currentRot, DeltaTime, droneData->ROTATION_SPEED);
	SetActorRotation(newRot);
}

void AK_Drone::InitializeDetectedUnitSlot()
{
	//CapsuleTrace로 유닛 탐지
	TArray<FHitResult> hitResults;
	FVector start = GetActorLocation();
	FVector end = start;
	
	float capsuleRadius = droneData->DRONE_DETECTION_INITIAL_RADIUS;
	float capsuleHalfHeight = droneData->DRONE_DETECTION_INITIAL_HALF_HEIGHT;
	FCollisionShape capsuleShpae = FCollisionShape::MakeCapsule(capsuleRadius, capsuleHalfHeight);
	
	GetWorld()->SweepMultiByChannel(
		hitResults,
		start, end,
		FQuat::Identity,
		ECC_Pawn,
		capsuleShpae);
	
#if WITH_EDITOR
	DrawDebugCapsule(
		GetWorld(),
		start,
		capsuleHalfHeight,
		capsuleRadius,
		FQuat::Identity,
		FColor::Blue,
		false,
		1.f, 0, 3.f
		);
#endif
	
	//현재 탐지된 유닛 캐싱
	TSet<AActor*> currentDetectedUnits;
	
	//탐지결과 처리
	for (const FHitResult& hit : hitResults)
	{
		AActor* detectedActor = hit.GetActor();
		AB_UnitBase* detectedUnit = Cast<AB_UnitBase>(detectedActor);
		
		if (detectedUnit)
		{
			currentDetectedUnits.Add(detectedActor);
			//broadcast
			onUnitDetected.Broadcast(detectedActor);
		}
	}
	
	previouslyDetectedUnits = currentDetectedUnits;
}

void AK_Drone::UpdateDetectedUnitSlot()
{
	//CapsuleTrace로 유닛 탐지
	TArray<FHitResult> hitResults;
	FVector start = GetActorLocation();
	FVector end = start;
	
	float capsuleRadius = droneData->DRONE_DETECTION_CAPSULE_RADIUS;
	float capsuleHalfHeight = droneData->DRONE_DETECTION_CAPSULE_HALF_HEIGHT;
	FCollisionShape capsuleShpae = FCollisionShape::MakeCapsule(capsuleRadius, capsuleHalfHeight);
	
	//(251217 수정) 캡슐 회전 추가
	FRotator currentRot = GetActorRotation();
	currentRot.Pitch += droneData->DRONE_DETECTION_CAPSULE_ROTATION;
	FQuat capsuleQuat = currentRot.Quaternion();
	
	GetWorld()->SweepMultiByChannel(
		hitResults,
		start, end,
		capsuleQuat,
		ECC_Pawn,
		capsuleShpae);
	
#if WITH_EDITOR
	DrawDebugCapsule(
		GetWorld(),
		start,
		capsuleHalfHeight,
		capsuleRadius,
		capsuleQuat,
		FColor::Green,
		false,
		1.f, 0, 3.f
		);
	
#endif
	
	//현재 탐지된 유닛 캐싱
	TSet<AActor*> currentDetectedUnits;
	TSet<AActor*> currentDetectedEnemys;
	
	//탐지결과 처리
	for (const FHitResult& hit : hitResults)
	{
		AActor* detectedActor = hit.GetActor();
		
		//유닛
		AB_UnitBase* detectedUnit = Cast<AB_UnitBase>(detectedActor);
		if (detectedUnit)
		{
			currentDetectedUnits.Add(detectedActor);
			//broadcast
			onUnitDetected.Broadcast(detectedActor);
		}
		
		//(251217 수정) 에너미 탐지 추가
		AP_EnemyBase* detectedEnemy = Cast<AP_EnemyBase>(detectedActor);
		if (detectedEnemy)
		{
			//KHS_SCREEN_INFO(TEXT("에너미 탐지되었음 - %p"), detectedEnemy);
			currentDetectedEnemys.Add(detectedActor);
			onUnitDetected.Broadcast(detectedActor);
		}

		AP_EnemyObstacle* detectedEnemyObstacle = Cast<AP_EnemyObstacle> ( detectedActor );
		if( detectedEnemyObstacle )
		{
			onUnitDetected.Broadcast ( detectedActor );
		}
	}
	
	//이전 탐지되었으나 지금은 안된 유닛들 UNDETECTED로 변경
	for (AActor* detectedActor : previouslyDetectedUnits)
	{
		if (!currentDetectedUnits.Contains(detectedActor))
		{
			//KHS_SCREEN_INFO(TEXT("에너미 탐지벗어남 - %p"), detectedActor);
			//broadcast
			onUnitLostDetection.Broadcast(detectedActor);
		}
	}
	
	//(251217 수정) 이전 탐지되었으나 지금은 안된 에너미들 브로드캐스트
	for (AActor* detectedEnemy : previouslyDetectedEnemys)
	{
		if (!currentDetectedEnemys.Contains(detectedEnemy))
		{
			onUnitLostDetection.Broadcast(detectedEnemy);
		}
	}
	
	//list update
	previouslyDetectedUnits = currentDetectedUnits;
	previouslyDetectedEnemys = currentDetectedEnemys;
}

// (20251223) W : 드론 스킬 용도 (Start)
void AK_Drone::StartSkill01Cooldown ()
{
	if (Skill01CooldownDuration <= 0.0f) return;

	GetWorldTimerManager ().SetTimer (
		Skill01CooldownTimerHandle ,
		this ,
		&AK_Drone::OnSkill01CooldownFinished ,
		Skill01CooldownDuration ,
		false
	);
}

void AK_Drone::StartSkill02Cooldown ()
{
	if (Skill02CooldownDuration <= 0.0f) return;

	GetWorldTimerManager ().SetTimer (
		Skill02CooldownTimerHandle ,
		this ,
		&AK_Drone::OnSkill02CooldownFinished ,
		Skill02CooldownDuration ,
		false
	);
}

void AK_Drone::OnSkill01CooldownFinished ()
{
	// 만료되면 inactive가 되긴 하지만, 명시적 Clear로 관리 편하게
	GetWorldTimerManager ().ClearTimer ( Skill01CooldownTimerHandle );
}

void AK_Drone::OnSkill02CooldownFinished ()
{
	GetWorldTimerManager ().ClearTimer ( Skill02CooldownTimerHandle );
}
// (20251223) W : 드론 스킬 용도 (End)

void AK_Drone::Move(const FVector2D& inputValue)
{
	moveInputValue = inputValue;
}

void AK_Drone::UpDown(float inputValue)
{
	upDownInputValue = inputValue;
}

void AK_Drone::UseSkill01()
{
	// (20251223) W : 드론 스킬 용도
	if (!IsSkill01Ready ())
	{
		return;
	}

	//previouslyDetectedUnits을 순회하면서 유닛들에게 회복명령
	for (AActor* actor : previouslyDetectedUnits)
	{
		AB_UnitBase* unit = Cast<AB_UnitBase>(actor);
		if (!unit || !unit->IsAlive())
		{
			continue;
		}
		
		unit->ReceiveSupport_HP(droneData->RECOVER_HEALTH_AMOUNT);
	}

	// (20251223) W : 드론 스킬 용도
	StartSkill01Cooldown ();
}

void AK_Drone::UseSkill02()
{
	// (20251223) W : 드론 스킬 용도
	if (!IsSkill02Ready ())
	{
		return;
	}

	for (AActor* actor : previouslyDetectedUnits)
	{
		AB_UnitBase* unit = Cast<AB_UnitBase>(actor);
		if (!unit || !unit->IsAlive())
		{
			continue;
		}
		
		unit->ReceiveSupport_Sanity(droneData->RECOVER_SANITY_AMOUNT);
	}

	// (20251223) W : 드론 스킬 용도
	StartSkill02Cooldown ();
}

// (20251223) W : 드론 스킬 용도 (Start)
bool AK_Drone::IsSkill01Ready () const
{
	const UWorld* World = GetWorld ();
	if (!World) return true;

	return !World->GetTimerManager ().IsTimerActive ( Skill01CooldownTimerHandle );
}

float AK_Drone::GetSkill01CooldownRemaining () const
{
	const UWorld* World = GetWorld ();
	if (!World) return 0.0f;

	const float Remaining = World->GetTimerManager ().GetTimerRemaining ( Skill01CooldownTimerHandle );
	return FMath::Max ( 0.0f , Remaining );
}

float AK_Drone::GetSkill01CooldownRatioRemaining () const
{
	if (Skill01CooldownDuration <= 0.0f) return 0.0f;
	return FMath::Clamp ( GetSkill01CooldownRemaining () / Skill01CooldownDuration , 0.0f , 1.0f );
}

bool AK_Drone::IsSkill02Ready () const
{
	const UWorld* World = GetWorld ();
	if (!World) return true;

	return !World->GetTimerManager ().IsTimerActive ( Skill02CooldownTimerHandle );
}

float AK_Drone::GetSkill02CooldownRemaining () const
{
	const UWorld* World = GetWorld ();
	if (!World) return 0.0f;

	const float Remaining = World->GetTimerManager ().GetTimerRemaining ( Skill02CooldownTimerHandle );
	return FMath::Max ( 0.0f , Remaining );
}

float AK_Drone::GetSkill02CooldownRatioRemaining () const
{
	if (Skill02CooldownDuration <= 0.0f) return 0.0f;
	return FMath::Clamp ( GetSkill02CooldownRemaining () / Skill02CooldownDuration , 0.0f , 1.0f );
}
// (20251223) W : 드론 스킬 용도 (End)

