#include "PJB/TimerUI/P_PhaseInfoWidget.h"

#include "LastRequiem.h"
#include "Components/TextBlock.h"
#include "PJB/System/P_GameStateBase.h"
#include "KHS/Drone/K_Drone.h"

void UP_PhaseInfoWidget::NativeConstruct ()
{
	Super::NativeConstruct ();

	Current_Wave->SetText ( FText::AsNumber ( 0 ) );
	Total_Wave->SetText ( FText::AsNumber ( 10 ) );
	Score->SetText ( FText::AsNumber ( 0 ) );

	AP_GameStateBase* GS = GetWorld () ? GetWorld ()->GetGameState<AP_GameStateBase> () : nullptr;
	if (GS)
	{
		GS->OnWaveStart.AddDynamic ( this , &UP_PhaseInfoWidget::RefreshWave );
		GS->OnAddScore.AddDynamic ( this , &UP_PhaseInfoWidget::RefreshScore );
	}
}

void UP_PhaseInfoWidget::RefreshWave ()
{
	AP_GameStateBase* GS = GetWorld () ? GetWorld ()->GetGameState<AP_GameStateBase> () : nullptr;
	if (GS)
	{
		if(GS->CurrentWave > GS->TotalWave)
		{
			Current_Wave->SetText ( FText::AsNumber ( GS->CurrentWave ) );
			Total_Wave->SetText ( FText::FromString ( TEXT ( "INF" ) ) );
		}
		else
		{
			Current_Wave->SetText ( FText::AsNumber ( GS->CurrentWave ) );
			Total_Wave->SetText ( FText::AsNumber ( GS->TotalWave ) );
		}
	}
}

void UP_PhaseInfoWidget::RefreshScore ( int32 InScore )
{
	AP_GameStateBase* GS = GetWorld () ? GetWorld ()->GetGameState<AP_GameStateBase> () : nullptr;
	if (GS)
	{
		Score->SetText ( FText::AsNumber ( GS->TotalScore ) );
	}
}
