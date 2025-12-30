#pragma once

#include "CoreMinimal.h"
#include "KHS/UI/K_BaseUIWidget.h"

#include "P_ScoreBlock.generated.h"

class UTextBlock;

UCLASS()
class LASTREQUIEM_API UP_ScoreBlock : public UK_BaseUIWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION ( BlueprintCallable , Category = "UI|Score" )
	void SetBlockData ( FText InTitle , int32 InCount , int32 InScore );

protected:
	UPROPERTY (meta = ( BindWidget ) )
	TObjectPtr<UTextBlock> Title;
	UPROPERTY ( meta = ( BindWidget ) )
	TObjectPtr<UTextBlock> Count;
	UPROPERTY ( meta = ( BindWidget ) )
	TObjectPtr<UTextBlock> Score;
};
