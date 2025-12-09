#include "PJB/Enemy/P_EnemyBase.h"

#include "PJB/Component/P_CombatComponent.h"

AP_EnemyBase::AP_EnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatComp = CreateDefaultSubobject<UP_CombatComponent> ( TEXT ( "Combat Component" ) );

}

void AP_EnemyBase::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic ( this , &AP_EnemyBase::OnTakeDamage );

}

void AP_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AP_EnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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
	bIsAlive = false;
	OnEnemyDieDelegate.Broadcast ();
}

