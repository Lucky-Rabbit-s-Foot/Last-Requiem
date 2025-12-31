#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PJB/Data/P_SpawnDataRow.h"
#include "PJB/Data/P_GameResultData.h"

#include "P_GameStateBase.generated.h"

UENUM ()
enum class EGamePhase : uint8
{
	Preparation,
	Wave,
	GameOver
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnGameEventSignature );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnAddScore , int32 , Score );

UCLASS()
class LASTREQUIEM_API AP_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AP_GameStateBase ();

protected:
	virtual void BeginPlay () override;

public:
	UFUNCTION()
	void CountEnemyKill ( int32 InEnemyID );
	UFUNCTION ()
	void CountRecoveryHealth ();
	UFUNCTION ()
	void CountRecoverySanity ();
	UFUNCTION ()
	void CountDestructEnemyObstacle ();

	UFUNCTION ()
	void AddScoreForInfo ( int32 InScore );
	UFUNCTION ()
	void AddTimeScore ();
	UFUNCTION ()
	void StopTimeScore ();
	FTimerHandle ScoreTimerHandle;

	FOnGameEventSignature OnGameStart;
	FOnGameEventSignature OnWaveStart;
	FOnGameEventSignature OnWaveStop;
	FOnGameEventSignature OnWaveEnd;
	FOnGameEventSignature OnGameOver;

	FOnAddScore OnAddScore;

	bool bIsWaveStop = false;

	UPROPERTY ( BlueprintReadOnly , Category = "GameFlow" )
	int32 CurrentWave = 0;
	UPROPERTY ( BlueprintReadOnly , Category = "GameFlow" )
	int32 TotalWave = 10;

	UPROPERTY ( BlueprintReadOnly , Category = "GameFlow" )
	float RemainingTime = 0.0f;

	UPROPERTY ( BlueprintReadOnly , Category = "GameFlow" )
	EGamePhase CurrentPhase = EGamePhase::Preparation;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "GameFlow" )
	UDataTable* CurrentWaveSpawnData = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "GameFlow" )
	float CurrentSpawnInterval = 15.0f;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "GameFlow" )
	float RealGameTimeSeconds = 0.0f;

	void SetPlayTime ();
	void RegisterFortress ( AActor* InFortress ) { Fortress = InFortress; }
	AActor* GetFortress () const { return Fortress.Get (); }

	FP_GameResultData GameResultData;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Score" )
	UP_ScoringDataAsset* ScoreModifier = nullptr;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Score" )
	int32 TotalScore = 0;

private:
	TWeakObjectPtr<AActor> Fortress;
};
