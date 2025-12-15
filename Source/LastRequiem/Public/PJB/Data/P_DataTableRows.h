#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "PJB/Data/P_EnemyDataAsset.h"

#include "P_DataTableRows.generated.h"

USTRUCT ( BlueprintType )
struct FP_EnemySpawnRow : public FTableRowBase
{
	GENERATED_BODY ()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag EnemyTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UP_EnemyDataAsset* EnemyDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> EnemyClass;
};