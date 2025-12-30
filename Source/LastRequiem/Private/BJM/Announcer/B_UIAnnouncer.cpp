// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/Announcer/B_UIAnnouncer.h"
#include "Kismet/GameplayStatics.h"
#include "PJB/System/P_GameStateBase.h"
#include "PJB/Fortress/P_Fortress.h" 

// Sets default values
AB_UIAnnouncer::AB_UIAnnouncer()
{
	PrimaryActorTick.bCanEverTick = false;

	bPlayed50 = false;
	bPlayed25 = false;
	bPlayed10 = false;
}

void AB_UIAnnouncer::PlayUnitDeathSound ( FString UnitName )
{
	if (UnitDeathSoundMap.Contains ( UnitName ))
	{
		PlayVoice ( UnitDeathSoundMap[UnitName] );
	}
}

// Called when the game starts or when spawned
void AB_UIAnnouncer::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld ();
	if (!World) return;

	AP_GameStateBase* GS = Cast<AP_GameStateBase> ( World->GetGameState () );
	if (GS)
	{
		GS->OnGameStart.AddDynamic ( this , &AB_UIAnnouncer::HandleGameStart );
		GS->OnWaveStart.AddDynamic ( this , &AB_UIAnnouncer::HandleWaveStart );
		GS->OnWaveEnd.AddDynamic ( this , &AB_UIAnnouncer::HandleWaveEnd );

		TryBindFortressDelegate ();

		//if (AActor* ActorFortress = GS->GetFortress ())
		//{
		//	if (AP_Fortress* MyFortress = Cast<AP_Fortress> ( ActorFortress ))
		//	{
		//		MyFortress->OnFortressDamagedDelegate.AddDynamic ( this , &AB_UIAnnouncer::HandleCoreDamaged );
		//		MyFortress->OnFortressBrokenDelegate.AddDynamic ( this , &AB_UIAnnouncer::HandleCoreBroken );
		//	}
		//}
	}

}

// Called every frame
void AB_UIAnnouncer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AB_UIAnnouncer::HandleGameStart ()
{
	PlayVoice ( GameStart_01 );

	float WaitTime = 0.5f;

	if (GameStart_01)
	{

		WaitTime = GameStart_01->GetDuration () + GameStart_Delay;
	}

	FTimerHandle TimerHandle;
	GetWorld ()->GetTimerManager ().SetTimer (
		TimerHandle ,
		this ,
		&AB_UIAnnouncer::PlayGameStart_Sequence2 ,
		WaitTime ,
		false
	);
}

void AB_UIAnnouncer::PlayGameStart_Sequence2 ()
{
	PlayVoice ( GameStart_02 );
}

void AB_UIAnnouncer::HandleWaveStart ()
{
	PlayVoice ( WaveStartSound );
}

void AB_UIAnnouncer::HandleWaveEnd ()
{
	PlayVoice ( WaveEndSound );
}

void AB_UIAnnouncer::HandleCoreDamaged ( AP_Fortress* Fortress )
{
	if (!Fortress) return;

	float Ratio = Fortress->GetHealthPercent ();

	if (Ratio <= 0.1f)
	{
		if (!bPlayed10)
		{
			PlayVoice ( CoreHP_10 );
			bPlayed10 = true;
			bPlayed25 = true;
			bPlayed50 = true;
		}
	}
	else if (Ratio <= 0.25f)
	{
		if (!bPlayed25)
		{
			PlayVoice ( CoreHP_25 );
			bPlayed25 = true;
			bPlayed50 = true;
		}
	}
	else if (Ratio <= 0.5f)
	{
		if (!bPlayed50)
		{
			PlayVoice ( CoreHP_50 );
			bPlayed50 = true;
		}
	}
}

void AB_UIAnnouncer::HandleCoreBroken ( AP_Fortress* Fortress )
{
	PlayVoice ( GameOver_CoreDestroyed );

	UE_LOG ( LogTemp , Warning , TEXT ( "아나운서: 코어 파괴 확인, 게임 종료 방송 송출" ) );
}

void AB_UIAnnouncer::PlayVoice ( USoundBase* Sound )
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D ( this , Sound );
	}
}

void AB_UIAnnouncer::TryBindFortressDelegate ()
{
	AP_GameStateBase* GS = Cast<AP_GameStateBase> ( GetWorld ()->GetGameState () );
	if (!GS) return;

	if (AActor* ActorFortress = GS->GetFortress ())
	{
		if (AP_Fortress* MyFortress = Cast<AP_Fortress> ( ActorFortress ))
		{
			MyFortress->OnFortressDamagedDelegate.AddDynamic ( this , &AB_UIAnnouncer::HandleCoreDamaged );
			MyFortress->OnFortressBrokenDelegate.AddDynamic ( this , &AB_UIAnnouncer::HandleCoreBroken );

			UE_LOG ( LogTemp , Warning , TEXT ( "아나운서: 요새 연결 성공!" ) ); // 확인용 로그
			return;
		}
	}

	FTimerHandle WaitHandle;
	GetWorld ()->GetTimerManager ().SetTimer (
		WaitHandle ,
		this ,
		&AB_UIAnnouncer::TryBindFortressDelegate ,
		0.1f ,
		false
	);
	UE_LOG ( LogTemp , Log , TEXT ( "아나운서: 요새 찾는 중..." ) );
}
