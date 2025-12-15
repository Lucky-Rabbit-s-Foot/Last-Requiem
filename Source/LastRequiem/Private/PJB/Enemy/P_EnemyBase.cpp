#include "PJB/Enemy/P_EnemyBase.h"

#include "Components/CapsuleComponent.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PJB/AI/P_AIControllerEnemyBase.h"
#include "PJB/Component/P_CombatComponent.h"
#include "KWB/Component/IndicatorSpriteComponent.h"

#include "Navigation/PathFollowingComponent.h"

AP_EnemyBase::AP_EnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComp = CreateDefaultSubobject<UP_CombatComponent> ( TEXT ( "Combat Component" ) );
	SpriteComp = CreateDefaultSubobject<UIndicatorSpriteComponent> ( TEXT ( "Sprite Component" ) );
	InitSpriteComponent ();

	static FGameplayTag EnemyTag = FGameplayTag::RequestGameplayTag ( FName ( "Enemy" ) );
	GameplayTags.AddTag ( EnemyTag );

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AP_EnemyBase::InitSpriteComponent ()
{
	SpriteComp->SetupAttachment ( GetRootComponent() );
	SpriteComp->SetRelativeLocation ( FVector ( 0.0f , 0.0f , 300.0f ) );
	SpriteComp->SetRelativeRotation ( FRotator ( -90.0f , 0.0f , 0.0f ) );
	SpriteComp->SetCastShadow ( false );
	SpriteComp->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
}

void AP_EnemyBase::BeginPlay()
{
	Super::BeginPlay();

	InitAnimInstance ();

	OnTakeAnyDamage.AddDynamic ( this , &AP_EnemyBase::OnTakeDamage );
}

void AP_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	AIPathVisualization ();
}

void AP_EnemyBase::AIPathVisualization ()
{
	// AI 경로 시각화
	AAIController* AIC = Cast<AAIController> ( GetController () );
	if (AIC->GetPathFollowingComponent () && AIC->GetPathFollowingComponent ()->GetPath ().IsValid ())
	{
		const TArray<FNavPathPoint>& PathPoints = AIC->GetPathFollowingComponent ()->GetPath ()->GetPathPoints ();

		for (int32 i = 0; i < PathPoints.Num () - 1; ++i)
		{
			DrawDebugLine (
				GetWorld () ,
				PathPoints[i].Location ,
				PathPoints[i + 1].Location ,
				FColor::Green ,
				false , -1.0f , 0 , 3.0f
			);
		}
	}
	// -----
}

void AP_EnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AP_EnemyBase::GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const
{
	TagContainer = GameplayTags;
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

void AP_EnemyBase::OnDie ()
{
	if (!bIsAlive)
	{
		return;
	}
	bIsAlive = false;

	OnDeactivate ();

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
	if (AIC)
	{
		AIC->StopMovement();
	}

	GetCapsuleComponent ()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent ()->SetCollisionResponseToAllChannels ( ECR_Ignore );

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();	
	GetCharacterMovement ()->SetComponentTickEnabled ( false );

	SetLifeSpan ( 5.0f );
}

void AP_EnemyBase::InitAnimInstance()
{
	if (GetMesh())
	{
		AnimInstance = GetMesh()->GetAnimInstance();
	}
}