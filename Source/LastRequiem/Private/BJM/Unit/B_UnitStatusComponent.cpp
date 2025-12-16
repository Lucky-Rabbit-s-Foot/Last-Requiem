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

	CurrentHP = MaxHP;
	CurrentSanity = StartSanity;

	if (OnHPChanged.IsBound ())
	{
		OnHPChanged.Broadcast ( CurrentHP , MaxHP );
	}

	if (OnSanityChanged.IsBound ())
	{
		OnSanityChanged.Broadcast ( CurrentSanity , MaxSanity );
	}

	UpdateBehaviorState ();

	
}

//// Called every frame
//void UB_UnitStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

void UB_UnitStatusComponent::ReduceHP (float InDamage)
{
	// 데미지 음수일 경우 무시(혹시몰라서 넣음)
	if (InDamage <= 0.0f) return;

	CurrentHP = FMath::Clamp(CurrentHP - InDamage, 0.0f, MaxHP);

	if (OnHPChanged.IsBound())
	{
		OnHPChanged.Broadcast(CurrentHP, MaxHP);
	}

	ModifySanity (-InDamage * 0.1f);

	
}

void UB_UnitStatusComponent::ModifySanity (float InAmount)
{
	CurrentSanity = FMath::Clamp(CurrentSanity + InAmount, 0.0f, MaxSanity);

	if (OnSanityChanged.IsBound())
	{
		OnSanityChanged.Broadcast(CurrentSanity, MaxSanity);
	}

	UpdateBehaviorState ();
}

void UB_UnitStatusComponent::SetCombatState(bool bNewState)
{

	if (bIsInCombat == bNewState) return;
	bIsInCombat = bNewState;

	if (bIsInCombat)
	{
		// 전투 종료 타이머
		GetWorld()->GetTimerManager().ClearTimer(CombatTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			CombatTimerHandle,
			this,
			&UB_UnitStatusComponent::ResetCombatState,
			5.0f,
			false
		);
		// 전투 중 정신력 감소 타이머
		GetWorld ()->GetTimerManager ().SetTimer (
			CombatSanityTimerHandle ,
			this ,
			&UB_UnitStatusComponent::ReduceSanityInCombat ,
			1.0f ,
			true
		);

		 UE_LOG(LogTemp, Warning, TEXT("전투 모드 활성화"));
	}
	else
	{
		// 전투 끝나면 정신력 감소 타이머도 끔
		GetWorld ()->GetTimerManager ().ClearTimer ( CombatSanityTimerHandle );
		UE_LOG ( LogTemp , Warning , TEXT ( "전투 종료: 정신력 감소 중지" ) );
	}

}

void UB_UnitStatusComponent::ResetCombatState()
{
	bIsInCombat = false;
	UE_LOG(LogTemp, Warning, TEXT("전투 모드 비활성화"));
}

void UB_UnitStatusComponent::ReduceSanityInCombat ()
{
	ModifySanity ( -1.0f );
}

void UB_UnitStatusComponent::UpdateBehaviorState ()
{
	EUnitBehaviorState NewState = EUnitBehaviorState::Normal;


	if (CurrentSanity <= 1.0f) 
	{
		NewState = EUnitBehaviorState::Madness;  
	}
	else if (CurrentSanity <= 10.0f)  
	{
		NewState = EUnitBehaviorState::Panic;     
	}
	else if (CurrentSanity <= 20.0f)  
	{
		NewState = EUnitBehaviorState::Fear;     
	}
	else if (CurrentSanity <= 40.0f)  
	{
		NewState = EUnitBehaviorState::Tense;  
	}
	else if (CurrentSanity <= 80.0f)  
	{
		NewState = EUnitBehaviorState::Normal;   
	}
	else 
	{
		NewState = EUnitBehaviorState::Awakened;
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





