// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/Drone/K_Drone.h"
#include "KHS/Data/K_DroneData.h"

#include "BJM/Unit/B_UnitBase.h"
#include "BJM/Unit/B_UnitMentalTypes.h"

#include "KWB/Component/IndicatorSpriteComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "KHS/Util/K_LoggingSystem.h"
#include "KWB/Component/IndicatorSpriteComponent.h"
#include "PJB/Enemy/P_EnemyBase.h"


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
}

void AK_Drone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(detectionTimerHandle);
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AK_Drone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDroneMovement(DeltaTime);
	UpdateDroneRotation(DeltaTime);
}

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
}

void AK_Drone::UpdateDroneMovement(float DeltaTime)
{
	if (!sphereComp)
	{
		return;
	}
	
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
		if (bOnLimitHeight)
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
	//previouslyDetectedUnits을 순회하면서 유닛들에게 회복명령
	for (AActor* actor : previouslyDetectedUnits)
	{
		AB_UnitBase* unit = Cast<AB_UnitBase>(actor);
		if (!unit || !unit->IsAlive())
		{
			KHS_WARN(TEXT("UseSkill01 : No Valid Unit Selected"));
			return;
		}
		
		unit->ReceiveSupport_HP(droneData->RECOVER_HEALTH_AMOUNT);
	}
}

void AK_Drone::UseSkill02()
{
	for (AActor* actor : previouslyDetectedUnits)
	{
		AB_UnitBase* unit = Cast<AB_UnitBase>(actor);
		if (!unit || !unit->IsAlive())
		{
			KHS_WARN(TEXT("UseSkill02 : No Valid Unit Selected"));
			return;
		}
		
		unit->ReceiveSupport_Sanity(droneData->RECOVER_SANITY_AMOUNT);
	}
}

