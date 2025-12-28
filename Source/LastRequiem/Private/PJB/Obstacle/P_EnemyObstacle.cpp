#include "PJB/Obstacle/P_EnemyObstacle.h"

#include "LastRequiem.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "KWB/Component/IndicatorSpriteComponent.h" // 포인트

AP_EnemyObstacle::AP_EnemyObstacle()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "Mesh" ) );
	SetRootComponent ( Mesh );

	Mesh->SetCollisionProfileName ( FName ( "NoCollision" ) );
	Mesh->SetCanEverAffectNavigation ( false );

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent> ( TEXT ( "CollisionComp" ) );
	CollisionComp->SetCollisionProfileName ( TEXT ( "BlockAll" ) );
	CollisionComp->SetupAttachment ( Mesh );

	SpriteComp = CreateDefaultSubobject<UIndicatorSpriteComponent> ( TEXT ( "SpriteComp" ) );
	SpriteComp->SetupAttachment ( Mesh );

	GeometryComp = CreateDefaultSubobject<UGeometryCollectionComponent> ( TEXT ( "GeometryComp" ) );
	GeometryComp->SetupAttachment ( Mesh );

	GeometryComp->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	GeometryComp->SetSimulatePhysics ( false );
	GeometryComp->SetVisibility ( false );

	AIControllerClass = nullptr;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::Disabled;
}

void AP_EnemyObstacle::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;
	OnTakeAnyDamage.AddDynamic ( this , &AP_EnemyObstacle::OnTakeDamage );

	Mesh->SetCanEverAffectNavigation ( false );

	static FGameplayTag ObstacleTag = FGameplayTag::RequestGameplayTag ( FName ( "Enemy.Obstacle" ) );
	GameplayTags.AddTag ( ObstacleTag );
}

void AP_EnemyObstacle::GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const
{
	TagContainer = GameplayTags;
}


void AP_EnemyObstacle::OnTakeDamage ( AActor* DamagedActor , float Damage , const UDamageType* DamageType , AController* InstigateBy , AActor* DamageCauser )
{
	if (bIsBroken) return;

	Health -= Damage;
	if (Health <= 0.0f)
	{
		OnBroken ();
	}
}

void AP_EnemyObstacle::OnBroken ()
{
	if (bIsBroken) return;
	bIsBroken = true;

	GameplayTags.Reset ();

	if (Mesh)
	{
		Mesh->SetVisibility ( false );
	}

	// 포인트
	if (SpriteComp)
	{
		SpriteComp->SetSpriteOnOff ( false );
	}

	if (CollisionComp)
	{
		CollisionComp->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	}

	if (GeometryComp)
	{
		GeometryComp->SetVisibility ( true );
		GeometryComp->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );
		GeometryComp->SetCollisionProfileName ( FName ( "Destructible" ) );
		GeometryComp->SetSimulatePhysics ( true );
	}

	if (MasterFieldClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld ()->SpawnActor<AActor> (
			MasterFieldClass ,
			GetActorLocation () ,
			GetActorRotation () ,
			SpawnParams
		);
	}

	OnEnemyObstacleBrokenDelegate.Broadcast ( this );
	SetLifeSpan ( 3.0f );
}
