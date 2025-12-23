#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PJB/Data/P_SpawnDataRow.h"

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

	void RegisterFortress ( AActor* InFortress ) { Fortress = InFortress; }
	AActor* GetFortress () const { return Fortress.Get (); }

	UFUNCTION ( BlueprintCallable , Category = "Data|Score" )
	void AddScore ( int32 Amount );
	UFUNCTION ( BlueprintPure , Category = "Data|Score" )
	int32 GetScore () const { return TotalScore; }

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = "Data|Score" )
	int32 TotalScore = 0;

private:
	TWeakObjectPtr<AActor> Fortress;
};
