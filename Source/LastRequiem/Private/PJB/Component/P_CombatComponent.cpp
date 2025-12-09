#include "PJB/Component/P_CombatComponent.h"

UP_CombatComponent::UP_CombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UP_CombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UP_CombatComponent::TakeDamage ( float InDamage )
{
	
}

void UP_CombatComponent::OnAttack ( AActor* Target )
{
}

void UP_CombatComponent::OnDie ()
{
}