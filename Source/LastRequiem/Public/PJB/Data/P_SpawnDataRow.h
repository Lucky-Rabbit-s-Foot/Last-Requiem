#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "PJB/Data/P_EnemyDataAsset.h"

#include "P_SpawnDataRow.generated.h"

USTRUCT ( BlueprintType )
struct FP_EnemySpawnRow : public FTableRowBase
{
	GENERATED_BODY ()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> EnemyClass = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UP_EnemyDataAsset* EnemyDataAsset = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 SpawnCount = 1;
};
