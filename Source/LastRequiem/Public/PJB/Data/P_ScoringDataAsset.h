#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "P_ScoringDataAsset.generated.h"

UCLASS()
class LASTREQUIEM_API UP_ScoringDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Scoring" )
	int32 PlayTime = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Scoring" )
	int32 Heal = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Scoring" )
	int32 Sanity = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Scoring" )
	int32 Dest = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Scoring" )
	int32 Melee = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Scoring" )
	int32 EnMelee = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Scoring" )
	int32 Range = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Scoring" )
	int32 EnRange = 0;
};
