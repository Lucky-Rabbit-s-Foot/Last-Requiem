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

UCLASS()
class LASTREQUIEM_API AP_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AP_GameStateBase ();

	UFUNCTION()
	void CountEnemyKill ( int32 InEnemyID );

	FOnGameEventSignature OnGameStart;
	FOnGameEventSignature OnWaveStart;
	FOnGameEventSignature OnWaveEnd;
	FOnGameEventSignature OnGameOver;

	UPROPERTY ( BlueprintReadOnly , Category = "GameFlow" )
	int32 CurrentWave = 0;

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

private:
	TWeakObjectPtr<AActor> Fortress;
};
