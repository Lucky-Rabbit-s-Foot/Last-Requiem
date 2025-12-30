#include "PJB/Fortress/P_Fortress.h"

#include "LR_GameMode.h"
#include "PJB/System/P_GameStateBase.h"
#include "KWB/Component/IndicatorSpriteComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AP_Fortress::AP_Fortress()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent> ( TEXT ( "Mesh" ) );
	SetRootComponent ( Mesh );

	SpriteComp = CreateDefaultSubobject<UIndicatorSpriteComponent> ( TEXT ( "SpriteComp" ) );
	SpriteComp->SetupAttachment ( Mesh );
	
	//GeometryComp = CreateDefaultSubobject<UGeometryCollectionComponent> ( TEXT ( "GeometryComp" ) );
	//GeometryComp->SetupAttachment ( Mesh );

	//GeometryComp->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	//GeometryComp->SetSimulatePhysics ( false );
	//GeometryComp->SetVisibility ( false );
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

	if (ALR_GameMode* GM = Cast<ALR_GameMode> ( GetWorld ()->GetAuthGameMode () ) )
	{
		OnFortressBrokenDelegate.AddDynamic ( GM , &ALR_GameMode::OnGameOver );
	}

	Mesh->SetCanEverAffectNavigation ( false );

	if (SpriteComp)
	{
		SpriteComp->SetRelativeRotation ( FRotator ( 0.0f , 90.0f , -90.0f ) );
		SpriteComp->SetSpriteOnOff ( true );
	}

	static FGameplayTag FortressTag = FGameplayTag::RequestGameplayTag ( FName ( "Fortress" ) );
	GameplayTags.AddTag ( FortressTag );
}

void AP_Fortress::Tick ( float DeltaTime )
{
	Super::Tick ( DeltaTime );

	if (bIsDissolving && DustEffect)
	{
		float DissolveSpeed = 2.0f;
		CurrentDissolveAmount += DeltaTime * DissolveSpeed;
		if(CurrentDissolveAmount > 1.0f)
		{
			bIsDissolving = false;
			if (Mesh)
			{
				Mesh->SetVisibility ( false );
			}
		}
	}
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
		OnFortressDamagedDelegate.Broadcast (this);
	}
}

void AP_Fortress::OnBroken ()
{
	if (bIsBroken) return;
	bIsBroken = true;

	GameplayTags.Reset ();

	//if (Mesh)
	//{
	//	Mesh->SetVisibility ( false );
	//}

	if (SpriteComp)
	{
		SpriteComp->SetSpriteOnOff ( false );
	}
	
	StartDissolve ();

	//if (GeometryComp)
	//{
	//	GeometryComp->SetVisibility ( true );
	//	GeometryComp->SetCollisionEnabled ( ECollisionEnabled::QueryAndPhysics );
	//	GeometryComp->SetCollisionProfileName ( FName ( "Destructible" ) );
	//	GeometryComp->SetSimulatePhysics ( true );
	//}

	//if (MasterFieldClass)
	//{
	//	FActorSpawnParameters SpawnParams;
	//	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//	GetWorld ()->SpawnActor<AActor> (
	//		MasterFieldClass ,
	//		GetActorLocation () ,
	//		GetActorRotation () ,
	//		SpawnParams
	//	);
	//}
	OnFortressBrokenDelegate.Broadcast ( this );
	SetLifeSpan ( LifeSpanTimeAfterBroken );
}

void AP_Fortress::StartDissolve()
{
	if (bIsDissolving) return;
	bIsDissolving = true;

	if (DustEffect)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached (
			DustEffect ,
			Mesh ,              // 요새 메쉬에 붙임
			FName ( "None" ) ,
			FVector::ZeroVector ,
			FRotator::ZeroRotator ,
			EAttachLocation::KeepRelativeOffset ,
			true
		);
	}
	Mesh->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
}
