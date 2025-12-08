// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "B_UnitMentalTypes.generated.h"


UENUM(BlueprintType)
enum class EUnitBehaviorState : uint8
{
	// 정신력 상
	Inspired    UMETA (DisplayName = "고양"),		// 충성도 상
	Cold        UMETA (DisplayName = "냉정"),		// 충성도 중
	Lazy        UMETA (DisplayName = "나태"),		// 충성도 하

	// 정신력 중
	Normal      UMETA (DisplayName = "노말") ,

	// 정신력 하
	Madness     UMETA (DisplayName = "광기"),		// 충성도 상
	Fear        UMETA (DisplayName = "공포"),		// 충성도 중
	Panic       UMETA (DisplayName = "패닉"),		// 충성도 하
};