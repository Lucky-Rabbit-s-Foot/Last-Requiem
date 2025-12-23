#include "PJB/Spawner/P_EnemySpawner.h"

#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "PJB/Data/P_SpawnDataRow.h"
#include "PJB/Data/P_SpawnerDataAsset.h"
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
	
	if (AP_GameStateBase* GS = GetWorld ()->GetGameState<AP_GameStateBase> ())
	{
		GS->OnWaveStart.AddDynamic ( this , &AP_EnemySpawner::StartSpawning );
		GS->OnWaveEnd.AddDynamic ( this , &AP_EnemySpawner::StopSpawning );
	}
}

void AP_EnemySpawner::StartSpawning ()
{
	AP_GameStateBase* GS = GetWorld ()->GetGameState<AP_GameStateBase> ();
	if (!GS) return;

	EnemyDataTable = GS->CurrentWaveSpawnData;
	if (!EnemyDataTable) return;

	GetWorldTimerManager ().SetTimer (
		SpawnTimerHandle ,
		this ,
		&AP_EnemySpawner::SpawnEnemy ,
		GS->CurrentSpawnInterval ,
		true ,
		0.0f
	);
}

void AP_EnemySpawner::StopSpawning ()
{
	GetWorldTimerManager ().ClearTimer ( SpawnTimerHandle );
}

void AP_EnemySpawner::SpawnEnemy ()
{
	if (!EnemyDataTable) return;
	
	FP_EnemySpawnRow* SelectedRow = nullptr;

	static const FString ContextString ( TEXT ( "EnemySpawnContext" ) );
	TArray<FP_EnemySpawnRow*> AllRows;
	EnemyDataTable->GetAllRows<FP_EnemySpawnRow> ( ContextString , AllRows );

	for(FP_EnemySpawnRow* Row : AllRows)
	{
		if (!Row) continue;

		SelectedRow = Row;
		if (!SelectedRow || !SelectedRow->EnemyClass || !SelectedRow->EnemyDataAsset) return;

		FVector SpawnLocation = GetActorLocation ();
		FTransform SpawnTransform = GetActorTransform ();

		if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent ( GetWorld () ))
		{
			FNavLocation RandomNavLocation;
			if (NavSys->GetRandomPointInNavigableRadius ( SpawnLocation , DA->SpawnRadius , RandomNavLocation ))
			{
				SpawnLocation = RandomNavLocation.Location;
				SpawnLocation.Z = GetActorLocation().Z + 100.0f;
			}
		}
		SpawnTransform.SetLocation ( SpawnLocation );

		for (int i = 0; i < SelectedRow->SpawnCount; ++i)
		{
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
				if (AK_Drone* Drone = Cast<AK_Drone> ( UGameplayStatics::GetPlayerPawn ( GetWorld () , 0 ) ))
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
}

#if WITH_EDITOR
void AP_EnemySpawner::PostEditChangeProperty ( FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty ( PropertyChangedEvent );

	if (SpawnRadiusVisualizer && DA)
	{
		SpawnRadiusVisualizer->SetSphereRadius ( DA->SpawnRadius );
	}
}
#endif