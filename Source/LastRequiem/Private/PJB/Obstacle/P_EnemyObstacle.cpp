#include "PJB/Obstacle/P_EnemyObstacle.h"

#include "LastRequiem.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "KWB/Component/IndicatorSpriteComponent.h" // 포인트
#include "KHS/Drone/K_Drone.h"
#include "PJB/System/P_GameStateBase.h"
#include "LR_GameMode.h"
#include "Components/WidgetComponent.h"
#include "PJB/Obstacle/P_DetectedObstacleWidget.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"

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

	DetectedWidget = CreateDefaultSubobject<UWidgetComponent> ( TEXT ( "EnemyWidgetComp" ) );
	DetectedWidget->SetupAttachment ( RootComponent );
	DetectedWidget->SetWidgetSpace ( EWidgetSpace::Screen );
	DetectedWidget->SetDrawAtDesiredSize ( true );

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

	SpriteComp->SetRelativeRotation ( FRotator ( 0.0f , 90.0f , -90.0f ) );
	SpriteComp->SetSpriteOnOff ( false );

	static FGameplayTag ObstacleTag = FGameplayTag::RequestGameplayTag ( FName ( "Enemy.Obstacle" ) );
	GameplayTags.AddTag ( ObstacleTag );

	if(ALR_GameMode* GM = Cast<ALR_GameMode> ( GetWorld ()->GetAuthGameMode () ) )
	{
		// 게임 모드의 디폴트 폰 가져오기
		AK_Drone* Drone = Cast<AK_Drone> ( UGameplayStatics::GetPlayerPawn ( GetWorld () , 0 ) );
		BindDrone ( Drone );
	}

	if(AP_GameStateBase* GS = GetWorld ()->GetGameState<AP_GameStateBase> ())
	{
		OnEnemyObstacleBrokenDelegate.AddDynamic ( GS , &AP_GameStateBase::CountDestructEnemyObstacle );
	}

	if (DetectedWidget)
	{
		DetectedWidget->SetVisibility ( true );

		UUserWidget* WidgetObj = DetectedWidget->GetUserWidgetObject ();
		if (WidgetObj)
		{
			CachedDetectedWidget = Cast<UP_DetectedObstacleWidget> ( WidgetObj );
		}
	}
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

	if (SpriteComp)
	{
		SpriteComp->SetSpriteOnOff ( false );
	}

	if (CollisionComp)
	{
		CollisionComp->SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	}

	if (DetectedWidget)
	{
		DetectedWidget->SetVisibility ( false );
	}

	if (DestructionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation (
			this ,
			DestructionEffect ,
			GetActorLocation () ,
			GetActorRotation ()
		);
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

	OnEnemyObstacleBrokenDelegate.Broadcast ();
	SetLifeSpan ( 3.0f );
}


void AP_EnemyObstacle::BindDrone ( AK_Drone* InDrone )
{
	if (!InDrone) return;
	InDrone->onUnitDetected.AddUObject ( this , &AP_EnemyObstacle::OnDetected );
	InDrone->onUnitLostDetection.AddUObject ( this , &AP_EnemyObstacle::OnLostDetection );
}

void AP_EnemyObstacle::OnDetected ( AActor* DetectedActor )
{
	if (DetectedActor != this || bIsBroken) return;
	
	if (bIsDetected) return;
	bIsDetected = true;

	if (SpriteComp)
	{
		SpriteComp->SetSpriteOnOff ( true );
	}

	if (!CachedDetectedWidget && DetectedWidget)
	{
		CachedDetectedWidget = Cast<UP_DetectedObstacleWidget> ( DetectedWidget->GetUserWidgetObject () );
	}

	if (CachedDetectedWidget)
	{
		if (DetectedWidget)
		{
			DetectedWidget->SetVisibility ( true );
		}
		CachedDetectedWidget->PlayShowAnimation ();
	}
}

void AP_EnemyObstacle::OnLostDetection ( AActor* DetectedActor )
{
	if (DetectedActor != this || bIsBroken) return;
	
	if (!bIsDetected) return;
	bIsDetected = false;

	if (CachedDetectedWidget)
	{
		CachedDetectedWidget->PlayHideAnimation ();
	}
	else if (DetectedWidget)
	{
		DetectedWidget->SetVisibility ( false );
	}
}