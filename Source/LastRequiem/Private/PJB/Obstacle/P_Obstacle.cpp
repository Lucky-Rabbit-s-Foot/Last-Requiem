#include "PJB/Obstacle/P_Obstacle.h"

AP_Obstacle::AP_Obstacle()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "Mesh" ) );
	SetRootComponent ( Mesh );

	static FGameplayTag ObstacleTag = FGameplayTag::RequestGameplayTag ( FName ( "Obstacle" ) );
	GameplayTags.AddTag ( ObstacleTag );
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
}

void AP_Obstacle::OnTakeDamage ( AActor* DamagedActor , float Damage , const UDamageType* DamageType , AController* InstigateBy , AActor* DamageCauser )
{
	if (bIsBroken) return;

	Health -= Damage;
	if (Health < 0.0f)
	{
		OnBroken ();
	}
}

void AP_Obstacle::OnBroken ()
{
	bIsBroken = true;
	OnObstacleBrokenDelegate.Broadcast ();
	Destroy ();
}
