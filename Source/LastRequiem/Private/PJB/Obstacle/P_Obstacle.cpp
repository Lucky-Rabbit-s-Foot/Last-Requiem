#include "PJB/Obstacle/P_Obstacle.h"

#include "LastRequiem.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
//#include "KWB/Component/IndicatorSpriteComponent.h"

AP_Obstacle::AP_Obstacle()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "Mesh" ) );
	SetRootComponent ( Mesh );
	Mesh->SetCollisionProfileName ( FName ( "NoCollision" ) );
	Mesh->SetCanEverAffectNavigation ( false );

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent> ( TEXT ( "CollisionComp" ) );
	CollisionComp->SetCollisionProfileName ( TEXT ( "BlockAll" ) );
	CollisionComp->SetupAttachment ( Mesh );

	// TODO: Activate later if fixed indicator sprite issue
//	SpriteComp = CreateDefaultSubobject<UIndicatorSpriteComponent> ( TEXT ( "SpriteComp" ) );
//	SpriteComp->SetupAttachment ( Mesh );

	GeometryComp = CreateDefaultSubobject<UGeometryCollectionComponent> ( TEXT ( "GeometryComp" ) );
	GeometryComp->SetupAttachment ( Mesh );

	GeometryComp->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	GeometryComp->SetSimulatePhysics ( false );
	GeometryComp->SetVisibility ( false );
}

void AP_Obstacle::GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const
{
	TagContainer = GameplayTags;
}

void AP_Obstacle::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;
	OnTakeAnyDamage.AddDynamic ( this , &AP_Obstacle::OnTakeDamage );

	Mesh->SetCanEverAffectNavigation ( false );

	static FGameplayTag ObstacleTag = FGameplayTag::RequestGameplayTag ( FName ( "Obstacle" ) );
	GameplayTags.AddTag ( ObstacleTag );
}

void AP_Obstacle::OnTakeDamage ( AActor* DamagedActor , float Damage , const UDamageType* DamageType , AController* InstigateBy , AActor* DamageCauser )
{
	if (bIsBroken) return;

	Health -= Damage;
	if (Health <= 0.0f)
	{
		OnBroken ();
	}
}

void AP_Obstacle::OnBroken ()
{
	LOG_SIMPLE ( Log );
	//if (!bIsBroken) return;
	//bIsBroken = true;

	if (Mesh)
	{
		Mesh->SetVisibility ( false );
	}

	//if (SpriteComp)
	//{
	//	SpriteComp->SetSpriteOnOff ( false );
	//}

	if (CollisionComp)
	{
		LOG_SIMPLE ( Log );
		CollisionComp->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	}

	if (GeometryComp)
	{
		LOG_SIMPLE ( Warning );

		GeometryComp->SetVisibility ( true );
		GeometryComp->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );
		GeometryComp->SetCollisionProfileName ( FName ( "Destructible" ) );
		GeometryComp->SetSimulatePhysics ( true );
	}

	if (MasterFieldClass)
	{
		LOG_SIMPLE ( Error );
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld ()->SpawnActor<AActor> (
			MasterFieldClass ,
			GetActorLocation () ,
			GetActorRotation () ,
			SpawnParams
		);
	}

	OnObstacleBrokenDelegate.Broadcast ( this );
	SetLifeSpan ( 3.0f );
}
