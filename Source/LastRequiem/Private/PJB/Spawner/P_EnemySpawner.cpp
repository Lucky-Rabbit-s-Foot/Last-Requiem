#include "PJB/Spawner/P_EnemySpawner.h"

#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "PJB/Data/P_DataTableRows.h"
#include "PJB/Enemy/P_EnemyBase.h"
#include "PJB/System/P_GameStateBase.h"
#include "KHS/Drone/K_Drone.h"

AP_EnemySpawner::AP_EnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	InitArrowComponentForFindSpawner ();

	SpawnRadiusVisualizer = CreateDefaultSubobject<USphereComponent> ( TEXT ( "SpawnRadiusVisualizer" ) );
	SpawnRadiusVisualizer->SetupAttachment ( RootComponent );
	
	SpawnRadiusVisualizer->SetCollisionProfileName ( TEXT ( "NoCollision" ) );
	SpawnRadiusVisualizer->SetHiddenInGame ( true );
	SpawnRadiusVisualizer->ShapeColor = FColor::White;
}

void AP_EnemySpawner::InitArrowComponentForFindSpawner ()
{
	ArrowComp = CreateDefaultSubobject<UArrowComponent> ( TEXT ( "ArrowComponent" ) );
	SetRootComponent ( ArrowComp );
	ArrowComp->ArrowColor = FColor::Red;
	ArrowComp->ArrowSize = 2.0f;
}

void AP_EnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	StartSpawnEnemy ();
}

void AP_EnemySpawner::StartSpawnEnemy ()
{
	if (SpawnInterval <= 0.0f || !EnemyDataTable) return;

	GetWorldTimerManager ().SetTimer (
		SpawnTimerHandle ,
		this ,
		&AP_EnemySpawner::SpawnEnemy ,
		SpawnInterval ,
		true ,
		SpawnDelay
	);
}


void AP_EnemySpawner::SpawnEnemy ()
{
	if (!EnemyDataTable || !EnemyTagToSpawn.IsValid ()) return;
	
	FP_EnemySpawnRow* SelectedRow = nullptr;

	static const FString ContextString ( TEXT ( "EnemySpawnContext" ) );
	TArray<FP_EnemySpawnRow*> AllRows;
	EnemyDataTable->GetAllRows<FP_EnemySpawnRow> ( ContextString , AllRows );

	for(FP_EnemySpawnRow* Row : AllRows)
	{
		if (!Row || !Row->EnemyTag.MatchesTag ( EnemyTagToSpawn )) continue;

		SelectedRow = Row;
		if (!SelectedRow || !SelectedRow->EnemyClass || !SelectedRow->EnemyDataAsset) return;

		FVector SpawnLocation = GetActorLocation ();
		FTransform SpawnTransform = GetActorTransform ();

		if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent ( GetWorld () ))
		{
			FNavLocation RandomNavLocation;
			if (NavSys->GetRandomPointInNavigableRadius ( SpawnLocation , SpawnRadius , RandomNavLocation ))
			{
				SpawnLocation = RandomNavLocation.Location;
				SpawnLocation.Z += 50.0f;
			}
		}
		SpawnTransform.SetLocation ( SpawnLocation );

		AP_EnemyBase* SpawnedEnemy = GetWorld ()->SpawnActorDeferred<AP_EnemyBase> (
			SelectedRow->EnemyClass ,
			SpawnTransform ,
			nullptr ,
			nullptr ,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);
		
		if (SpawnedEnemy)
		{
			SpawnedEnemy->InitEnemyData ( SelectedRow->EnemyDataAsset );
			if (AK_Drone* Drone = Cast<AK_Drone> ( UGameplayStatics::GetPlayerPawn ( GetWorld () , 0 ) ) )
			{
				SpawnedEnemy->BindDrone ( Drone );
			}

			UGameplayStatics::FinishSpawningActor ( SpawnedEnemy , SpawnTransform );
			if (SpawnedEnemy->GetController () == nullptr)
			{
				SpawnedEnemy->SpawnDefaultController ();
			}
		}
	}
}

void AP_EnemySpawner::OnConstruction ( const FTransform& Transform )
{
	Super::OnConstruction ( Transform );
	if (SpawnRadiusVisualizer)
	{
		SpawnRadiusVisualizer->SetSphereRadius ( SpawnRadius );
	}
}
