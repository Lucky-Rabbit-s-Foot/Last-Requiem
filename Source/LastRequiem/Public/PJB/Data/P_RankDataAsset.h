#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "P_RankDataAsset.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FRankData
{
	GENERATED_BODY()
public:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	TObjectPtr<UTexture2D> RankImage;
	
	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	int32 MinScoreRequirement;
};

UCLASS()
class LASTREQUIEM_API UP_RankDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	// S to F : Top Rank is 0 , Bottom Rank is RankList.Num() - 1
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Rank" )
	TArray<FRankData> RankList;
};
