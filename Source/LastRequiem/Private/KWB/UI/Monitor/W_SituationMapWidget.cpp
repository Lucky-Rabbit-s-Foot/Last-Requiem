// Fill out your copyright notice in the Description page of Project Settings.

#include "KWB/UI/Monitor/W_SituationMapWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "KHS/Util/K_LoggingSystem.h"
#include "PJB/Fortress/P_Fortress.h"

UW_SituationMapWidget::UW_SituationMapWidget ()
{
	UILayer = EUILayer::POPUP;
	zOrder = 0;
}

void UW_SituationMapWidget::NativeConstruct()
{
	Super::NativeConstruct ();

	if (FortressHPBar)
	{
		FortressHPBar->SetPercent ( 0.0f );
	}

	BindFortressDelegates ();

	if (Exit)
	{
		Exit->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleExitButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Exit이 존재하지 않습니다." ) );
	}

	if (Attack)
	{
		Attack->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleAttackButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Attack이 존재하지 않습니다." ) );
	}

	if (Stop)
	{
		Stop->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleStopButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Stop이 존재하지 않습니다." ) );
	}

	if (Hold)
	{
		Hold->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleHoldButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Hold이 존재하지 않습니다." ) );
	}

	if (Retreat)
	{
		Retreat->OnClicked.AddDynamic ( this , &UW_SituationMapWidget::HandleRetreatButtonClicked );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Retreat이 존재하지 않습니다." ) );
	}
}

void UW_SituationMapWidget::NativeDestruct ()
{
	if (AP_Fortress* Fortress = CachedFortress.Get ())
	{
		Fortress->OnFortressDamagedDelegate.RemoveDynamic ( this , &UW_SituationMapWidget::HandleFortressDamaged );
		Fortress->OnFortressBrokenDelegate.RemoveDynamic ( this , &UW_SituationMapWidget::HandleFortressBroken );
	}
	CachedFortress.Reset ();

	if (Exit)
	{
		Exit->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleExitButtonClicked );
		//KHS_INFO ( TEXT ( "Exit->OnClicked.RemoveDynamic" ) );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Exit이 존재하지 않습니다." ) );
	}

	if (Attack)
	{
		Attack->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleAttackButtonClicked );
		//KHS_INFO ( TEXT ( "Attack->OnClicked.RemoveDynamic" ) );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Attack이 존재하지 않습니다." ) );
	}

	if (Stop)
	{
		Stop->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleStopButtonClicked );
		//KHS_INFO ( TEXT ( "Stop->OnClicked.RemoveDynamic" ) );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Stop이 존재하지 않습니다." ) );
	}

	if (Hold)
	{
		Hold->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleHoldButtonClicked );
		//KHS_INFO ( TEXT ( "Hold->OnClicked.RemoveDynamic" ) );

	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Hold이 존재하지 않습니다." ) );
	}

	if (Retreat)
	{
		Retreat->OnClicked.RemoveDynamic ( this , &UW_SituationMapWidget::HandleRetreatButtonClicked );
		KHS_INFO ( TEXT ( "Retreat->OnClicked.RemoveDynamic" ) );
	}
	else
	{
		UE_LOG ( LogTemp , Error , TEXT ( "TObjectPtr<UButton> Retreat이 존재하지 않습니다." ) );
	}
}

void UW_SituationMapWidget::NativeTick ( const FGeometry& MyGeometry , float InDeltaTime )
{
	Super::NativeTick ( MyGeometry , InDeltaTime );

	if (!bInterpFortressHP || !FortressHPBar)
	{
		return;
	}

	CurrentFortressHPPercent = FMath::FInterpTo (
		CurrentFortressHPPercent ,
		TargetFortressHPPercent ,
		InDeltaTime ,
		FortressHPInterpSpeed
	);

	FortressHPBar->SetPercent ( CurrentFortressHPPercent );

	if (FMath::IsNearlyEqual ( CurrentFortressHPPercent , TargetFortressHPPercent , 0.001f ))
	{
		CurrentFortressHPPercent = TargetFortressHPPercent;
		FortressHPBar->SetPercent ( CurrentFortressHPPercent );
		bInterpFortressHP = false;
	}
}

void UW_SituationMapWidget::HandleExitButtonClicked ()
{
	UE_LOG ( LogTemp , Log , TEXT ( "Exit Button Clicked." ) );
	onCloseUIRequested.Broadcast ( this );
}

void UW_SituationMapWidget::HandleAttackButtonClicked ()
{
	OnAttackButtonClicked.Broadcast ();
	UE_LOG ( LogTemp , Log , TEXT ( "Attack Button Clicked." ) );
}

void UW_SituationMapWidget::HandleStopButtonClicked ()
{
	OnStopButtonClicked.Broadcast ();
	UE_LOG ( LogTemp , Log , TEXT ( "Stop Button Clicked." ) );
}

void UW_SituationMapWidget::HandleHoldButtonClicked ()
{
	OnHoldButtonClicked.Broadcast ();
	UE_LOG ( LogTemp , Log , TEXT ( "Hold Button Clicked." ) );
}

void UW_SituationMapWidget::HandleRetreatButtonClicked ()
{
	OnRetreatButtonClicked.Broadcast ();
	UE_LOG ( LogTemp , Log , TEXT ( "Retreat Button Clicked." ) );
}

void UW_SituationMapWidget::HandleFortressDamaged ( AP_Fortress* InFortress )
{
	if (!FortressHPBar)
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "W_SituationMapWidget: FortressHPBar가 nullptr 입니다. (WBP 변수 체크/BindWidget 확인)" ) );
		return;
	}

	AP_Fortress* Fortress = CachedFortress.Get ();
	if (!IsValid ( Fortress ))
	{
		TargetFortressHPPercent = 0.0f;
		bInterpFortressHP = true;
		return;
	}

	TargetFortressHPPercent = FMath::Clamp ( Fortress->GetHealthPercent () , 0.0f , 1.0f );
	bInterpFortressHP = true;
}

void UW_SituationMapWidget::HandleFortressBroken ( AP_Fortress* InFortress )
{
	TargetFortressHPPercent = 0.0f;
	bInterpFortressHP = true;
}

void UW_SituationMapWidget::BindFortressDelegates ()
{
	if (CachedFortress.IsValid ())
	{
		return;
	}

	UWorld* World = GetWorld ();
	if (!World)
	{
		return;
	}

	AP_Fortress* Fortress = Cast<AP_Fortress> (
		UGameplayStatics::GetActorOfClass ( World , AP_Fortress::StaticClass () )
	);

	if (!IsValid ( Fortress ))
	{
		UE_LOG ( LogTemp , Warning , TEXT ( "W_SituationMapWidget: Fortress를 찾지 못해 FortressHPBar가 갱신되지 않습니다." ) );
		return;
	}

	CachedFortress = Fortress;
	Fortress->OnFortressDamagedDelegate.AddDynamic ( this , &UW_SituationMapWidget::HandleFortressDamaged );
	Fortress->OnFortressBrokenDelegate.AddDynamic ( this , &UW_SituationMapWidget::HandleFortressBroken );

	HandleFortressDamaged ();
}
