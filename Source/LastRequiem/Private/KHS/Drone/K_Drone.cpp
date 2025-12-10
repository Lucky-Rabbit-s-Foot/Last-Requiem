// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/Drone/K_Drone.h"
#include "KHS/Data/K_DroneData.h"

#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AK_Drone::AK_Drone()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitializeComponents();
	
	bUseControllerRotationYaw = true;
	
	SetReplicates(true);
	SetReplicateMovement(true);
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
	
	//SFX settings
	
	//UI Settings
	// if (droneUIFactory)
	// {
	// 	droneUI = CreateWidget<UUserWidget>(GetWorld(), droneUIFactory);
	// 	if (droneUI)
	// 	{
	// 		droneUI->AddToViewPort(0);
	// 		
	// 	}
	// }
}

// Called every frame
void AK_Drone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDroneMovement(DeltaTime);
	UpdateDroneRotation(DeltaTime);
}

void AK_Drone::InitializeComponents()
{
	//sphere 
	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(100.f);
	
	//sphere 물리설정 추가
	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	sphereComp->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	sphereComp->SetCollisionResponseToAllChannels(ECR_Block);
	sphereComp->SetSimulatePhysics(true);
	sphereComp->SetEnableGravity(false); 
	sphereComp->SetLinearDamping(1.2f);  //관성 감쇠
	sphereComp->SetAngularDamping(2.0f); //회전 감쇠
	sphereComp->SetMassOverrideInKg(NAME_None, DRONE_MASS_WEIGHT); //드론 질량
	
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
		
		FVector horizontalForce = (forwardDir * moveInputValue.Y + rightDir * moveInputValue.X) * droneData->DRONE_HORIZONTAL_FORCE;
		FVector verticalForce = upDir * upDownInputValue * droneData->DRONE_VERTICAL_FORCE;
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


void AK_Drone::Move(const FVector2D& inputValue)
{
	moveInputValue = inputValue;
}

void AK_Drone::UpDown(float inputValue)
{
	upDownInputValue = inputValue;
}

void AK_Drone::ChangeMode()
{
}

void AK_Drone::UseSkill01()
{
}

void AK_Drone::UseSkill02()
{
}

