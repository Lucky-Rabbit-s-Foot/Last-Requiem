// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataTable.h"
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
	UTexture2D* ProfileImage = nullptr;

	// 유닛 이름
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	FText UnitName;

	// 체력
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	float MaxHP = 100.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	float CurrentHP = 100.0f;

	// 정신력
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	float MaxSanity = 100.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	float CurrentSanity = 80.0f;

	// 사망
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	bool bIsAlive = true;

	// 전투
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	bool bIsInCombat = false;

	// 음성상태
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Profile" )
	bool bIsSpeaking = false;


};

UENUM ( BlueprintType )
enum class EUnitType : uint8
{
	Jack	UMETA ( DisplayName = "Jack" ) ,
	James	UMETA ( DisplayName = "James" ) ,
	Bruno   UMETA ( DisplayName = "Bruno" ) ,
	Silas   UMETA ( DisplayName = "Silas" ) ,
	Viper   UMETA ( DisplayName = "Viper" ) ,
	Hans	UMETA ( DisplayName = "Hans" ) ,

};

USTRUCT ( BlueprintType )
struct FUnitDataTableRow : public FTableRowBase
{
	GENERATED_BODY ()

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	FText UnitName;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	UTexture2D* ProfileImage;

};

UENUM ( BlueprintType )
enum class EUnitVoiceEvent : uint8
{
	SpotEnemy   UMETA ( DisplayName = "Spot Enemy" ) ,   // 적 발견
	Combat      UMETA ( DisplayName = "Combat/Attack" ) ,// 공격/전투
	Recovery    UMETA ( DisplayName = "Recovery" ) ,     // 회복 받음
	Death       UMETA ( DisplayName = "Death" )         // 사망
};

USTRUCT ( BlueprintType )
struct FUnitVoiceProfile
{
	GENERATED_BODY ()

	// 적 발견했을 때
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	USoundBase* SpotEnemySound;

	// 공격할 때
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	USoundBase* AttackSound;

	// 회복 받았을 때
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	USoundBase* RecoverySound;

	// 죽었을 때
	UPROPERTY ( EditAnywhere , BlueprintReadWrite )
	USoundBase* DeathSound;
};