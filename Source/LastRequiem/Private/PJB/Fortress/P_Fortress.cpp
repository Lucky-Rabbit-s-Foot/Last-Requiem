#include "PJB/Fortress/P_Fortress.h"

AP_Fortress::AP_Fortress()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AP_Fortress::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;
	OnTakeAnyDamage.AddDynamic ( this , &AP_Fortress::OnTakeDamage );
}


void AP_Fortress::OnTakeDamage ( AActor* DamagedActor , float Damage , const UDamageType* DamageType , AController* InstigateBy , AActor* DamageCauser )
{
	if (bIsBroken) return;

	Health -= Damage;
	if (Health < 0.0f)
	{
		OnBroken ();
	}
	else
	{
		OnFortressDamagedDelegate.Broadcast ();
	}
}

void AP_Fortress::OnBroken ()
{
	bIsBroken = true;
	OnFortressBrokenDelegate.Broadcast ();
	Destroy ();
}
