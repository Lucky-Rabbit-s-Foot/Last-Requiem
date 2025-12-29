#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "P_TutorialImageDataAsset.generated.h"

UCLASS()
class LASTREQUIEM_API UP_TutorialImageDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Tutorial Data" )
	TArray<UTexture2D*> Images;
};
