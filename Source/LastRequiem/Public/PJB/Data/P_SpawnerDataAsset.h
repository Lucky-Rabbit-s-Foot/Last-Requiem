#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "P_SpawnerDataAsset.generated.h"

UCLASS()
class LASTREQUIEM_API UP_SpawnerDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Spawner" )
	float SpawnDelay = 1.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Spawner" )
	float SpawnInterval = 5.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|SpawnSettings" )
	float SpawnRadius = 300.0f;
};
