#include "LR_GameMode.h"

#include "LastRequiem.h"
#include "LR_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PJB/Data/P_SpawnDataRow.h"
#include "PJB/Data/P_WaveDataRow.h"
#include "PJB/System/P_GameStateBase.h"

ALR_GameMode::ALR_GameMode ()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALR_GameMode::BeginPlay ()
{
	Super::BeginPlay ();

	TotalWaves = WaveDataTable ? WaveDataTable->GetRowNames ().Num () : 0;
	
	if(AP_GameStateBase* GS = GetGameState<AP_GameStateBase>())
	{
		GS->OnGameStart.Broadcast();
	}
	StartPreparation ();
}

void ALR_GameMode::Tick ( float DeltaSeconds )
{
	Super::Tick ( DeltaSeconds );

	if(AP_GameStateBase* GS = GetGameState<AP_GameStateBase>())
	{
		if (GetWorldTimerManager ().IsTimerActive ( PhaseTimerHandle ))
		{
			GS->RemainingTime = GetWorldTimerManager ().GetTimerRemaining ( PhaseTimerHandle );
		}
	}
}

void ALR_GameMode::StartPreparation ()
{
	LOG_MESSAGE_SCREEN ( Log, TEXT ( "Preparation Phase Started! Next Wave in %.0f seconds." ) , PreparationTime );

	AP_GameStateBase* GS = GetGameState<AP_GameStateBase> ();
	if (!GS) return;

	GS->CurrentPhase = EGamePhase::Preparation;
	GetWorldTimerManager ().SetTimer ( 
		PhaseTimerHandle , 
		this , 
		&ALR_GameMode::StartWave , 
		PreparationTime , 
		false 
	);
}

void ALR_GameMode::StartWave ()
{
	LOG_MESSAGE_SCREEN ( Log , TEXT ( "Wave %d Started! Survive for %.0f seconds." ) , GetGameState<AP_GameStateBase> ()->CurrentWave + 1 , WaveTime );

	AP_GameStateBase* GS = GetGameState<AP_GameStateBase> ();
	if (!GS) return;

	GS->CurrentWave++;
	GS->CurrentPhase = EGamePhase::Wave;
	
	int32 LookupWave = FMath::Min ( GS->CurrentWave , TotalWaves );
	FName RowName = FName ( *FString::Printf ( TEXT ( "Wave%d" ) , LookupWave ) );
	if (WaveDataTable)
	{
		FP_WaveDataRow* WaveConfig = WaveDataTable->FindRow<FP_WaveDataRow> ( RowName , TEXT ( "WaveLogic" ) );

		if (WaveConfig && WaveConfig->EnemySpawnTable)
		{
			GS->CurrentSpawnInterval = WaveConfig->SpawnInterval;
			GS->CurrentWaveSpawnData = WaveConfig->EnemySpawnTable;
		}
	}
	
	GS->OnWaveStart.Broadcast ();

	GetWorldTimerManager ().SetTimer ( 
		PhaseTimerHandle , 
		this , 
		&ALR_GameMode::EndWave , 
		WaveTime , 
		false 
	);
}

void ALR_GameMode::EndWave ()
{
	LOG_MESSAGE_SCREEN ( Log , TEXT ( "Wave %d Ended!" ) , GetGameState<AP_GameStateBase> ()->CurrentWave );

	AP_GameStateBase* GS = GetGameState<AP_GameStateBase> ();
	if (!GS) return;

	GS->OnWaveEnd.Broadcast ();
	StartPreparation ();
}

void ALR_GameMode::OnGameOver ()
{
	LOG_MESSAGE_SCREEN ( Log , TEXT ( "Game Over!" ) );

	AP_GameStateBase* GS = GetGameState<AP_GameStateBase> ();
	ULR_GameInstance* GI = Cast<ULR_GameInstance> ( GetGameInstance () );
	if (!GS || !GI) return;
	
	GS->OnGameOver.Broadcast ();
	
	GI->GameResultCount.SetCountResult ( GS->GameResultData );
	GI->GameResultCount.SetScoreResult ( GS->GameResultData, ScoringDataAsset );
	//UGameplayStatics::OpenLevel ( this , FName ( TEXT ( "GameResultMap" ) ) );
}
