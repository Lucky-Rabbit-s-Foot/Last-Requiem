// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "B_MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API AB_MainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay () override;
	
};
