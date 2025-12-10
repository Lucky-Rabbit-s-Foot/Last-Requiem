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
	float DRONE_HORIZONTAL_FORCE = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Flight Speed"))
	float DRONE_VERTICAL_FORCE = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Deceleration Index"))
	float ROTATION_SPEED = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Deceleration Index"))
	float MAX_SPEED = 2000.f;
	
};
