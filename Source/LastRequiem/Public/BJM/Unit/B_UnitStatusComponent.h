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

	// 체력
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Unit|Stat")
	float MaxHP = 100.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Stat")
	float CurrentHP;

	// 정신력
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Unit|Stat")
	float MaxSanity = 100.0f;
	UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Unit|Stat")
	float StartSanity = 80.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Stat")
	float CurrentSanity;


	// 유닛 멘탈 상황
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Stat")
	EUnitBehaviorState CurrentState;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Unit|Status" )
	bool bIsInCombat = false;

	UPROPERTY ( VisibleAnywhere , BlueprintReadWrite , Category = "Unit|Status" )
	bool bIsSpeaking = false;

	UPROPERTY (BlueprintAssignable)
	FOnStateChanged OnStateChanged;

	UPROPERTY (BlueprintAssignable)
	FOnStatChanged OnHPChanged;

	UPROPERTY (BlueprintAssignable)
	FOnStatChanged OnSanityChanged;


	UFUNCTION (BlueprintCallable, Category = "Unit|Logic")
	void ReduceHP ( float InDamage );

	UFUNCTION (BlueprintCallable, Category = "Unit|Logic")
	void ModifySanity (float InAmount);

	UFUNCTION (BlueprintCallable, Category = "Unit|Logic")
	void SetCombatState ( bool bNewState );

protected:
	FTimerHandle CombatTimerHandle;
	void ResetCombatState ();

protected:
	FTimerHandle CombatSanityTimerHandle;

	void ReduceSanityInCombat ();
		
private:
	void UpdateBehaviorState ();

};
