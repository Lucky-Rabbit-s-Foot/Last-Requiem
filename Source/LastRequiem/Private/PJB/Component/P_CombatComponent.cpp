#include "PJB/Component/P_CombatComponent.h"

#include "Kismet/GameplayStatics.h"

UP_CombatComponent::UP_CombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UP_CombatComponent::BeginPlay()
{
	Super::BeginPlay();

	InitPrimitiveValues();
}


void UP_CombatComponent::TakeDamage(float InDamage)
{
	Health -= InDamage;
	if (Health <= 0.0f)
	{
		Health = 0.0f;
		OnDie();
	}
}

void UP_CombatComponent::OnAttack(AActor* Target)
{
	bIsCombat = true;
	
	UE_LOG ( LogTemp , Warning , TEXT ( "Attack Target: %s" ) , *Target->GetName () );

	UGameplayStatics::ApplyDamage(
		Target,
		AttackPower,
		GetOwner()->GetInstigatorController(),
		GetOwner(),
		UDamageType::StaticClass()
	);
}

void UP_CombatComponent::OnDie()
{
	bIsAlive = false;
}

void UP_CombatComponent::InitPrimitiveValues ()
{
	Health = MaxHealth;
	bIsAlive = true;
	bIsCombat = false;
}