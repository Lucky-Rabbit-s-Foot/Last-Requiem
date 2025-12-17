#include "PJB/Enemy/P_EnemyBase.h"

#include "Components/CapsuleComponent.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PJB/AI/P_AIControllerEnemyBase.h"
#include "PJB/Component/P_CombatComponent.h"
#include "PJB/Data/P_EnemyDataAsset.h"
#include "PJB/Enemy/P_AnimInstanceEnemyBase.h"
#include "KWB/Component/IndicatorSpriteComponent.h"
#include "BrainComponent.h"
#include "KHS/Drone/K_Drone.h"
#include "Kismet/GameplayStatics.h"
#include "PJB/System/P_GameStateBase.h"

#include "Navigation/PathFollowingComponent.h"

AP_EnemyBase::AP_EnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComp = CreateDefaultSubobject<UP_CombatComponent> ( TEXT ( "Combat Component" ) );
	SpriteComp = CreateDefaultSubobject<UIndicatorSpriteComponent> ( TEXT ( "Indicator Sprite Component" ) );
	SpriteComp->SetupAttachment ( GetRootComponent () );

	InitRotationSetting ();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void AP_EnemyBase::BeginPlay ()
{
	Super::BeginPlay ();

	SpriteComp->SetSpriteOnOff ( false );
	InitGameplayTag ();
	OnTakeAnyDamage.AddDynamic ( this , &AP_EnemyBase::OnTakeDamage );
}

void AP_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AP_EnemyBase::InitGameplayTag ()
{
	static FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag ( FName ( "Enemy" ) );
	GameplayTags.AddTag ( EnemyTag );
}

void AP_EnemyBase::InitRotationSetting ()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement ()->bOrientRotationToMovement = true;
	GetCharacterMovement ()->RotationRate = FRotator ( 0.0f , 640.0f , 0.0f );
}

void AP_EnemyBase::GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const
{
	TagContainer = GameplayTags;
}

void AP_EnemyBase::InitEnemyData(UP_EnemyDataAsset* InData)
{
	if (!InData) return;

	if(InData->SkeletalMesh)
	{
		GetMesh()->SetSkeletalMesh ( InData->SkeletalMesh );
	}

	if(InData->AnimInstanceClass)
	{
		GetMesh()->SetAnimInstanceClass ( InData->AnimInstanceClass );
	}
		
	GetCharacterMovement ()->MaxWalkSpeed = InData->BaseSpeed;
	BaseMoveSpeed = InData->BaseSpeed;
	CombatMoveSpeed = InData->CombatSpeed;

	CachedAttackRange = InData->AttackRange;
	CachedAttackMontage = InData->AttackMontage;

	Score = InData->Score;

	if (CombatComp)
	{
		CombatComp->InitStats ( InData->MaxHealth , InData->AttackRange , InData->AttackPower );
	}
}

void AP_EnemyBase::BindDrone ( AK_Drone* InDrone )
{
	InDrone->onUnitDetected.AddUObject ( this , &AP_EnemyBase::OnDetected );
	InDrone->onUnitLostDetection.AddUObject ( this , &AP_EnemyBase::OnLostDetection );
}

void AP_EnemyBase::OnAttack ()
{
	if (AP_AIControllerEnemyBase* AIC = Cast<AP_AIControllerEnemyBase> ( GetController () ))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent ())
		{
			AActor* TargetActor = Cast<AActor> ( BB->GetValueAsObject ( FName ( "TargetActor" ) ) );
			if (TargetActor)
			{
				CombatComp->OnAttack ( TargetActor );
			}
		}
	}
}

void AP_EnemyBase::OnTakeDamage ( AActor* DamagedActor , float Damage , const UDamageType* DamageType , AController* InstigateBy , AActor* DamageCauser )
{
	if (!bIsAlive) return;

	CombatComp->TakeDamage ( Damage );
	if (!CombatComp->IsAlive ())
	{
		OnDie ();
	}
}

void AP_EnemyBase::OnDetected ( AActor* DetectedActor )
{
	if (DetectedActor != this || !bIsAlive) return;

	if (SpriteComp)
	{
		SpriteComp->SetSpriteOnOff ( true );
	}
}

void AP_EnemyBase::OnLostDetection ( AActor* DetectedActor )
{
	if (DetectedActor != this || !bIsAlive) return;

	// TODO: if other units still detect this enemy then do not turn off the sprite

	if (SpriteComp)
	{
		SpriteComp->SetSpriteOnOff ( false );
	}
}

void AP_EnemyBase::OnDie ()
{
	if (!bIsAlive)
	{
		return;
	}
	bIsAlive = false;

	OnDeactivate ();

	if (AP_GameStateBase* GS = GetWorld () ? GetWorld ()->GetGameState<AP_GameStateBase> () : nullptr)
	{
		GS->AddScore ( Score );
	}

	OnEnemyDieDelegate.Broadcast ( this );
}

void AP_EnemyBase::OnDeactivate()
{
	GameplayTags.RemoveTag ( FGameplayTag::RequestGameplayTag ( FName ( "Enemy" ) ) );

	if (SpriteComp)
	{
		SpriteComp->SetIndicatorState ( EIndicatorSpriteState::Dead );
		SpriteComp->StopGlow ();
	}

	AP_AIControllerEnemyBase* AIC = Cast<AP_AIControllerEnemyBase>(GetController());
	if (AIC && AIC->GetBrainComponent())
	{
		AIC->GetBrainComponent ()->StopLogic ( TEXT ( "Enemy Dead" ) );
		AIC->StopMovement();
	}

	GetCapsuleComponent ()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent ()->SetCollisionResponseToAllChannels ( ECR_Ignore );

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	GetMesh ()->SetCollisionResponseToChannel ( ECC_Visibility , ECR_Ignore );
	GetMesh ()->SetCollisionResponseToChannel ( ECC_Camera , ECR_Ignore );
	GetMesh ()->SetCollisionResponseToChannel ( ECC_Pawn , ECR_Ignore );

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();	
	GetCharacterMovement ()->SetComponentTickEnabled ( false );

	if(USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		if(UP_AnimInstanceEnemyBase* AnimInst = Cast<UP_AnimInstanceEnemyBase>(SkeletalMeshComp->GetAnimInstance()))
		{
			AnimInst->SetDeadState ( true );
		}
	}

	SetLifeSpan ( 5.0f );
}

void AP_EnemyBase::SetCombatState ( bool bNewIsCombat )
{
	if (bIsCombat == bNewIsCombat) return;
	bIsCombat = bNewIsCombat;

	if (!CombatComp) return;
	if (bIsCombat)
	{
		GetCharacterMovement ()->MaxWalkSpeed = CombatMoveSpeed;
	}
	else
	{
		GetCharacterMovement ()->MaxWalkSpeed = BaseMoveSpeed;
	}

	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh ())
	{
		if (UP_AnimInstanceEnemyBase* AnimInst = Cast<UP_AnimInstanceEnemyBase> ( SkeletalMeshComp->GetAnimInstance () ))
		{
			AnimInst->SetCombatState ( bIsCombat );
		}
	}
}
