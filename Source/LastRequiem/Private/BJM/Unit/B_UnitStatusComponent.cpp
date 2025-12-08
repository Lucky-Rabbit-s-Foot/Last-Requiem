// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/Unit/B_UnitStatusComponent.h"

// Sets default values for this component's properties
UB_UnitStatusComponent::UB_UnitStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UB_UnitStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	// 스탯 초기설정
	CurrentHP = MaxHP;
	CurrentSanity = MaxSanity;
	CurrentLoyalty = MaxLoyalty;

	UpdateBehaviorState ();

	
}

//// Called every frame
//void UB_UnitStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UB_UnitStatusComponent::ApplyDamage (float InDamage)
{
	// 데미지 음수일 경우 무시(혹시몰라서 넣음)
	if (InDamage <= 0.0f) return;

	CurrentHP = FMath::Clamp ( CurrentHP - InDamage , 0.0f , MaxHP );

	if (OnHPChanged.IsBound())
	{
		OnHPChanged.Broadcast(CurrentHP, MaxHP);
	}

	ModifySanity (-InDamage * 0.1f);
}

void UB_UnitStatusComponent::ModifySanity (float InAmount)
{
	CurrentSanity = FMath::Clamp ( CurrentSanity + InAmount , 0.0f , MaxSanity );

	if (OnSanityChanged.IsBound ())
	{
		OnSanityChanged.Broadcast ( CurrentSanity , MaxSanity );
	}

	UpdateBehaviorState ();
}

void UB_UnitStatusComponent::ModifyLoyalty (float InAmount)
{
	CurrentLoyalty = FMath::Clamp ( CurrentLoyalty + InAmount , 0.0f , MaxLoyalty );

	UpdateBehaviorState ();
}

void UB_UnitStatusComponent::UpdateBehaviorState ()
{
	EUnitBehaviorState NewState = EUnitBehaviorState::Normal;

	if(bSanityHigh) // 정신력 상
	{
		if(bLoyaltyLow)
		{
			NewState = EUnitBehaviorState::Lazy;	 // 나태
		}
		else if(bLoyaltyHigh)
		{
			NewState = EUnitBehaviorState::Inspired; // 고양
		}
		else
		{
			NewState = EUnitBehaviorState::Cold;	 // 냉정
		}
	}
	else if (bSanityLow) // 정신력 하
	{
		if (bLoyaltyLow)
		{
			NewState = EUnitBehaviorState::Panic;	 // 패닉
		}
		else if (bLoyaltyHigh)
		{
			NewState = EUnitBehaviorState::Madness;	 // 광기
		}
		else
		{
			NewState = EUnitBehaviorState::Fear;	 // 공포
		}
	}
	else // 정신력 중
	{
		NewState = EUnitBehaviorState::Normal;		 // 평범
	}

	// 3. 상태 변경 시 방송
	if (CurrentState != NewState)
	{
		CurrentState = NewState;
		if (OnStateChanged.IsBound())
		{
			OnStateChanged.Broadcast(CurrentState);
		}
	}

}

