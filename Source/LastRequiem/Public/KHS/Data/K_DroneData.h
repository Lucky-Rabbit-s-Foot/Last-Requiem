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
	//MOVE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Flight Horizontal Speed"))
	float DRONE_HORIZONTAL_FORCE = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Flight Vertical Speed"))
	float DRONE_VERTICAL_FORCE = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Mesh Rotation Speed"))
	float ROTATION_SPEED = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Flight Max Speed"))
	float MAX_SPEED = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Move", meta=(ClampMin="0.0", ToolTip = "Drone Flight Max Height"))
	float DRONE_FLIGHT_MAX_HEIGHT = 2000.f;
	
	// (20251224) P : 드론 이동 (Start)
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "LR|Move" , meta = (ClampMin = "0.0" , ToolTip = "Drone Flight Min Height") )
	float DRONE_FLIGHT_MIN_HEIGHT = 400.f;
	// (20251224) P : 드론 이동 (End)

	//DETECTION
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Detection", meta=(ClampMin="0.0", ToolTip = "Drone Detection Initial Capsule Radius"))
	float DRONE_DETECTION_INITIAL_RADIUS = 10000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Detection", meta=(ClampMin="0.0", ToolTip = "Drone Detection Initial Capsule Half Height"))
	float DRONE_DETECTION_INITIAL_HALF_HEIGHT = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Detection", meta=(ClampMin="0.0", ToolTip = "Drone Detection Capsule Radius"))
	float DRONE_DETECTION_CAPSULE_RADIUS = 700.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Detection", meta=(ClampMin="0.0", ToolTip = "Drone Detection Capsule Half Height"))
	float DRONE_DETECTION_CAPSULE_HALF_HEIGHT = 2800.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Detection", meta=(ClampMin="0.0", ToolTip = "Drone Detection Capsule Rotate Degree"))
	float DRONE_DETECTION_CAPSULE_ROTATION = 60.f;
	
	//SKILL
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Skill", meta=(ClampMin="0.0", ToolTip = "Drone Skill - Unit Recover Health Amount"))
	float RECOVER_HEALTH_AMOUNT = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LR|Skill", meta=(ClampMin="0.0", ToolTip = "Drone Skill - Unit Recover Sanity Amount"))
	float RECOVER_SANITY_AMOUNT = 100.f;
	
};
