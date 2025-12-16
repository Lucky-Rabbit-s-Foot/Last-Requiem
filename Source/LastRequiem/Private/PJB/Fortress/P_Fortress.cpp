#include "PJB/Fortress/P_Fortress.h"

#include "PJB/System/P_GameStateBase.h"

AP_Fortress::AP_Fortress()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "Mesh" ) );
	SetRootComponent ( Mesh );

}

void AP_Fortress::GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const
{
	TagContainer = GameplayTags;
}

void AP_Fortress::BeginPlay()
{
	Super::BeginPlay();
	
	if (AP_GameStateBase* GS = GetWorld ()->GetGameState<AP_GameStateBase> ())
	{
		GS->RegisterFortress ( this );
	}

	Health = MaxHealth;
	OnTakeAnyDamage.AddDynamic ( this , &AP_Fortress::OnTakeDamage );

	Mesh->SetCanEverAffectNavigation ( false );

	static FGameplayTag FortressTag = FGameplayTag::RequestGameplayTag ( FName ( "Fortress" ) );
	GameplayTags.AddTag ( FortressTag );
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
		OnFortressDamagedDelegate.Broadcast ( this );
	}
}

void AP_Fortress::OnBroken ()
{
	bIsBroken = true;
	OnFortressBrokenDelegate.Broadcast ( this );
	Destroy ();
}
