#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"

#include "P_EnemySpawner.generated.h"

class UP_SpawnerDataAsset;

UCLASS()
class LASTREQUIEM_API AP_EnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AP_EnemySpawner();

	void InitArrowComponentForFindSpawner ();

	UFUNCTION ( BlueprintCallable , Category = "Factory" )
	void SpawnEnemies ();
	void SpawnEachEnemy (struct FP_EnemySpawnRow* SelectedRow , FTransform& SpawnTransform );

#if WITH_EDITOR
	virtual void PostEditChangeProperty ( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif

protected:
	virtual void BeginPlay() override;

	UFUNCTION ()
	void StartSpawning ();
	UFUNCTION ()
	void StopSpawning ();

protected:
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<class USphereComponent> SpawnRadiusVisualizer;
		
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|SpawnSettings" )
	UDataTable* EnemyDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly , Category = "Data|SpawnSettings" )
	UP_SpawnerDataAsset* DA;
	
	FTimerHandle SpawnTimerHandle;

private:
	UPROPERTY ( VisibleAnywhere , Category = "Visual" )
	class UArrowComponent* ArrowComp = nullptr;
};
