// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/W_DroneSkillWidget.h"
#include "TimerManager.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "KHS/Drone/K_Drone.h"

void UW_DroneSkillWidget::NativeConstruct ()
{
	Super::NativeConstruct ();

	// InitWithDrone를 밖에서 호출 안 해도, 소유 Pawn이 드론이면 자동 연결 시도
	if (!Drone.IsValid ())
	{
		if (APawn* P = GetOwningPlayerPawn ())
		{
			if (AK_Drone* AsDrone = Cast<AK_Drone> ( P ))
			{
				Drone = AsDrone;
			}
		}
	}

	// 초기 상태: 사용 가능(퍼센트 0)
	if (Skill01PGBar) Skill01PGBar->SetPercent ( 0.f );
	if (Skill02PGBar) Skill02PGBar->SetPercent ( 0.f );

	// 초기 아이콘 (Ready)
	SetBorderIcon ( Skill01Border , Skill01_ReadyIcon );
	SetBorderIcon ( Skill02Border , Skill02_ReadyIcon );

	// Tick 없이 UI 타이머로만 갱신
	if (UWorld* World = GetWorld ())
	{
		World->GetTimerManager ().SetTimer (
			UIUpdateTimerHandle ,
			this ,
			&UW_DroneSkillWidget::UpdateDroneUI ,
			0.03f , // 0.03~0.05 추천
			true
		);
	}
}

void UW_DroneSkillWidget::NativeDestruct ()
{
	if (UWorld* World = GetWorld ())
	{
		World->GetTimerManager ().ClearTimer ( UIUpdateTimerHandle );
	}
	Super::NativeDestruct ();
}

void UW_DroneSkillWidget::UpdateDroneUI ()
{
	if (!Drone.IsValid ())
	{
		if (Skill01PGBar) Skill01PGBar->SetPercent ( 0.f );
		if (Skill02PGBar) Skill02PGBar->SetPercent ( 0.f );

		if (bSkill01CoolingDownLast) { bSkill01CoolingDownLast = false; SetBorderIcon ( Skill01Border , Skill01_ReadyIcon ); }
		if (bSkill02CoolingDownLast) { bSkill02CoolingDownLast = false; SetBorderIcon ( Skill02Border , Skill02_ReadyIcon ); }
		return;
	}

	const bool bSkill01Ready = Drone->IsSkill01Ready ();
	const float Skill01Ratio = Drone->GetSkill01CooldownRatioRemaining ();

	const bool bSkill02Ready = Drone->IsSkill02Ready ();
	const float Skill02Ratio = Drone->GetSkill02CooldownRatioRemaining ();

	UpdateSkillSlot (
		1 , bSkill01Ready , Skill01Ratio ,
		Skill01Border , Skill01PGBar ,
		Skill01_ReadyIcon , Skill01_CooldownIcon ,
		bSkill01CoolingDownLast
	);

	UpdateSkillSlot (
		2 , bSkill02Ready , Skill02Ratio ,
		Skill02Border , Skill02PGBar ,
		Skill02_ReadyIcon , Skill02_CooldownIcon ,
		bSkill02CoolingDownLast
	);
}

void UW_DroneSkillWidget::InitWithDrone ( AK_Drone* InDrone )
{
	Drone = InDrone;
}

void UW_DroneSkillWidget::UpdateSkillSlot ( int32 SkillIndex , bool bReadyNow , float CooldownRatioRemaining , UBorder* Border , UProgressBar* ProgressBar , UTexture2D* ReadyIcon , UTexture2D* CooldownIcon , bool& bLastCoolingDownState )
{
	if (!ProgressBar) return;

	const bool bCoolingDownNow = !bReadyNow;

	// 요구사항:
	// - 사용 가능: 0
	// - 사용 순간: 1
	// - 쿨 동안 1 -> 0 감소
	ProgressBar->SetPercent ( bCoolingDownNow ? FMath::Clamp ( CooldownRatioRemaining , 0.f , 1.f ) : 0.f );

	// 아이콘 스왑(상태 바뀔 때만)
	if (bCoolingDownNow != bLastCoolingDownState)
	{
		bLastCoolingDownState = bCoolingDownNow;
		SetBorderIcon ( Border , bCoolingDownNow ? CooldownIcon : ReadyIcon );
	}
}

void UW_DroneSkillWidget::SetBorderIcon ( UBorder* Border , UTexture2D* Tex )
{
	if (Border && Tex)
	{
		Border->SetBrushFromTexture ( Tex );
	}
}
