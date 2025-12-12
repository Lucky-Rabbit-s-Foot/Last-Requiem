// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "BaseMapPaperSpriteComponent.generated.h"

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UBaseMapPaperSpriteComponent : public UPaperSpriteComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY ( EditAnywhere , Category = "Data|Image" )
	TObjectPtr<class UImage> ImageList

protected:


private:


};
