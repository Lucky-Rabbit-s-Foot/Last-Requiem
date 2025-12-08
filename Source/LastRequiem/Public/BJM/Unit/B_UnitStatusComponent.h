// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "B_UnitMentalTypes.h"
#include "B_UnitStatusComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnStateChanged, EUnitBehaviorState, NewState );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams (FOnStatChanged, float, Current, float, Max );

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LASTREQUIEM_API UB_UnitStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UB_UnitStatusComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// =================================== [스탯 설정] ===================================

	// 최대 체력
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Unit|Stat")
	float MaxHP = 100.0f;

	// 현재 체력
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Stat")
	float CurrentHP;

	// 최대 정신력
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Unit|Stat")
	float MaxSanity = 100.0f;

	// 현제 정신력
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Stat")
	float CurrentSanity;

	// 최대 충성심
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Unit|Stat")
	float MaxLoyalty = 100.0f;

	// 현재 충성심
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Stat")
	float CurrentLoyalty;

	// 유닛 멘탈 상황
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Stat")
	EUnitBehaviorState CurrentState;

	// =================================== [이 벤 트] ===================================

	UPROPERTY (BlueprintAssignable)
	FOnStateChanged OnStateChanged;

	UPROPERTY (BlueprintAssignable)
	FOnStatChanged OnHPChanged;

	UPROPERTY (BlueprintAssignable)
	FOnStatChanged OnSanityChanged;

	// =================================== [ 함    수] ===================================

	UFUNCTION (BlueprintCallable, Category = "Unit|Logic")
	void ApplyDamage (float InDamage);

	UFUNCTION (BlueprintCallable, Category = "Unit|Logic")
	void ModifySanity (float InAmount);

	UFUNCTION (BlueprintCallable, Category = "Unit|Logic")
	void ModifyLoyalty (float InAmount);

private:
	void UpdateBehaviorState ();




		
};
