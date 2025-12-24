#pragma once

#include "CoreMinimal.h"

#include "P_GameResultData.generated.h"

USTRUCT(BlueprintType)
struct FP_GameResultData
{
	GENERATED_BODY ()
public:
	void ResetResult ();
	void SetCountResult ( const FP_GameResultData& Count );
	void SetScoreResult ( const FP_GameResultData& Count , const class UP_ScoringDataAsset* DA );
	void LogResult () const;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 PlayTime = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 Heal = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 Sanity = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 Dest = 0;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 Melee = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 EnMelee = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 Range = 0;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 EnRange = 0;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	int32 TotalScore = 0;
};
