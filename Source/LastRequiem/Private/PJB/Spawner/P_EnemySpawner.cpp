#include "PJB/Spawner/P_EnemySpawner.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PJB/Data/P_DataTableRows.h"


AP_EnemySpawner::AP_EnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	ArrowComp = CreateDefaultSubobject<UArrowComponent> ( TEXT ( "ArrowComponent" ) );
	SetRootComponent ( ArrowComp );
	ArrowComp->ArrowColor = FColor::Red;
	ArrowComp->ArrowSize = 2.0f;
	
}

void AP_EnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (SpawnInterval > 0.0f && EnemyDataTable)
	{
		GetWorldTimerManager ().SetTimer ( 
			SpawnTimerHandlde , 
			this , 
			&AP_EnemySpawner::SpawnEnemy , 
			SpawnInterval , 
			true , 
			SpawnDelay 
		);
	}

}


void AP_EnemySpawner::SpawnEnemy ()
{
	if (!EnemyDataTable)
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "EnemyDataTable is null in EnemySpawner: %s" ) , *GetName () );
		return;
	}

	if(!EnemyTagToSpawn.IsValid())
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "EnemyTagToSpawn is invalid in EnemySpawner: %s" ) , *GetName () );
		return;
	}

	FEnemySpawnRow* SelectedRow = nullptr;

	static const FString ContextString ( TEXT ( "EnemySpawnContext" ) );
	TArray<FEnemySpawnRow*> AllRows;
	EnemyDataTable->GetAllRows<FEnemySpawnRow> ( ContextString , AllRows );

	for(FEnemySpawnRow* Row : AllRows)
	{
		if (Row && Row->EnemyTag.MatchesTag ( EnemyTagToSpawn ))
		{
			SelectedRow = Row;
			break;
		}
	}

	if (!SelectedRow || !SelectedRow->EnemyClass)
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "No matching enemy found for tag in EnemySpawner: %s" ) , *GetName () );
		return;
	}

	FTransform SpawnTransform = GetActorTransform ();
	
	APawn* SpawnedEnemy = GetWorld ()->SpawnActorDeferred<APawn> (
		SelectedRow->EnemyClass , 
		SpawnTransform , 
		nullptr , 
		nullptr , 
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn 
	);

	if (SpawnedEnemy)
	{
		// initialize any propertiese on enemy

		UGameplayStatics::FinishSpawningActor ( SpawnedEnemy , SpawnTransform );
		if (SpawnedEnemy->GetController() == nullptr)
		{
			SpawnedEnemy->SpawnDefaultController ();
		}
	}
}