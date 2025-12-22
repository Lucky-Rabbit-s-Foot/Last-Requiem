#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PJB/Data/P_SpawnDataRow.h"

#include "P_WaveDataRow.generated.h"

USTRUCT ( BlueprintType )
struct FP_WaveDataRow : public FTableRowBase
{
	GENERATED_BODY ()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText WaveName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnInterval = 15.0f;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	UDataTable* EnemySpawnTable = nullptr;

};
