// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "K_DroneData.generated.h"

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UK_DroneData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Flight Speed"))
	float DRONE_MAX_SPEED = 300.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Vertical Speed"))
	float DRONE_MAX_VERTICAL_SPEED = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Acceleration Index"))
	float ACCELERATION = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Deceleration Index"))
	float DECELERATION = 10.0f;
	
};
