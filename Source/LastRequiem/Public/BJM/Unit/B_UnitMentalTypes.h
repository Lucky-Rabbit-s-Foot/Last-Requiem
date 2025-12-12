// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h" 
#include "UObject/ScriptMacros.h" 
#include "B_UnitMentalTypes.generated.h"


UENUM(BlueprintType)
enum class EUnitBehaviorState : uint8
{
	// 정신력 00~01
	Madness     UMETA ( DisplayName = "광기" ) ,	
	// 정신력 01~10
	Panic       UMETA ( DisplayName = "패닉" ) ,	
	// 정신력 10~20
	Fear        UMETA ( DisplayName = "공포" ) ,	
	// 정신력 20~40
	Tense		UMETA ( DisplayName = "긴장" ) ,
	// 정신력 40~80
	Normal      UMETA ( DisplayName = "노말" ) ,
	// 정신력 80~100
	Awakened    UMETA ( DisplayName = "각성" ) ,

};

UENUM ( BlueprintType )
enum class EUnitCommandType : uint8
{
	None        UMETA ( DisplayName = "None" ) ,
	Move        UMETA ( DisplayName = "Move" ) ,
	AttackMove  UMETA ( DisplayName = "AttackMove" ) ,
	Hold        UMETA ( DisplayName = "Hold" ) ,
	Retreat     UMETA ( DisplayName = "Retreat" ),
	Stop		UMETA ( DisplayName = "Stop" ),
};

USTRUCT ( BlueprintType )
struct FUnitProfileData
{
	GENERATED_BODY ()

public:
	// 프로필사진
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	UTexture2D* ProfileImage;

	// 유닛 이름
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	FText UnitName;

	// 체력
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	float MaxHP;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	float CurrentHP;

	// 정신력
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	float MaxSanity;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	float CurrentSanity;

	// 사망
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	bool bIsAlive;

	// 전투
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	bool bIsInCombat;

	// 음성상태
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	bool bIsSpeaking;


};