#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "P_CopyrightData.generated.h"

USTRUCT ( BlueprintType )
struct FCopyrightInfo
{
	GENERATED_BODY ()

public:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	FString AssetName;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	FString License;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly )
	FString Author;
};

UCLASS ()
class LASTREQUIEM_API UP_CopyrightData : public UDataAsset
{
	GENERATED_BODY ()

public:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data" )
	TArray<FCopyrightInfo> CopyrightList;
};
