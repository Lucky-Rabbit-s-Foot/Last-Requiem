#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "P_EnemyDataAsset.generated.h"

UCLASS()
class LASTREQUIEM_API UP_EnemyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Visual" )
	USkeletalMesh* SkeletalMesh;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Visual" )
	TSubclassOf<UAnimInstance> AnimInstanceClass;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|ID" )
	int32 EnemyID = 0;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Movement" )
	float BaseSpeed = 300.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Movement" )
	float CombatSpeed = 300.0f;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Health" )
	float MaxHealth = 100.0f;
	
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Combat" )
	float AttackRange = 150.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Combat" )
	float AttackPower = 150.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Combat" )
	UAnimMontage* AttackMontage;
};
