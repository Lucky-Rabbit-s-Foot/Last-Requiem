#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"

#include "P_EnemySpawner.generated.h"

UCLASS()
class LASTREQUIEM_API AP_EnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AP_EnemySpawner();

	void InitArrowComponentForFindSpawner ();

	UFUNCTION ( BlueprintCallable , Category = "Factory" )
	void SpawnEnemy ();

protected:
	virtual void BeginPlay() override;

	void StartSpawnEnemy ();

protected:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|SpawnSettings" )
	FGameplayTag EnemyTagToSpawn;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|SpawnSettings" )
	UDataTable* EnemyDataTable;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|SpawnSettings" )
	float SpawnDelay = 1.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|SpawnSettings" )
	float SpawnInterval = 5.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|SpawnSettings" )
	float SpawnRadius = 500.0f;

	FTimerHandle SpawnTimerHandle;

private:
	UPROPERTY ( VisibleAnywhere , Category = "Visual" )
	class UArrowComponent* ArrowComp = nullptr;
};
