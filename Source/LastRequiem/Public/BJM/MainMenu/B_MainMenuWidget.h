// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "B_MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UB_MainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION ( BlueprintCallable )
	void OnClickNewGame ();

	UFUNCTION ( BlueprintCallable )
	void OnClickExitGame ();

protected:
	UFUNCTION ( BlueprintImplementableEvent )
	void PlayGameStartAnimation ();

};
