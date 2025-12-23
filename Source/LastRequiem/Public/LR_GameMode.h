#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "LR_GameMode.generated.h"

UCLASS()
class LASTREQUIEM_API ALR_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALR_GameMode ();
	virtual void Tick ( float DeltaSeconds ) override;

protected:
	virtual void BeginPlay () override;


protected:
	UPROPERTY ( EditDefaultsOnly , Category = "Data|Wave" )
	float PreparationTime = 10.0f;
	UPROPERTY ( EditDefaultsOnly , Category = "Data|Wave" )
	float WaveTime = 20.0f;

	int32 TotalWaves = 0;
private:
	FTimerHandle PhaseTimerHandle;

	UPROPERTY ( EditDefaultsOnly , Category = "Data|Wave" )
	UDataTable* WaveDataTable;

	void StartPreparation ();
	void StartWave ();
	void EndWave ();
	void OnGameOver ();
};
