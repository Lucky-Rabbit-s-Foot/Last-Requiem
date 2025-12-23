#include "PJB/TimerUI/P_TimerWidget.h"

#include "Components/TextBlock.h"
#include "PJB/System/P_GameStateBase.h"

void UP_TimerWidget::NativeConstruct ()
{
	Super::NativeConstruct ();
	GameStateBase = GetWorld () ? GetWorld ()->GetGameState<AP_GameStateBase> () : nullptr;
}

void UP_TimerWidget::NativeTick ( const FGeometry& MyGeometry , float InDeltaTime )
{
	Super::NativeTick ( MyGeometry , InDeltaTime );

	if (GameStateBase && TimerText)
	{
		float RemainingTime = GameStateBase->RemainingTime;
		RemainingTime = FMath::Max ( 0.0f , RemainingTime );

		int32 Minutes = FMath::FloorToInt ( RemainingTime / 60.0f );
		int32 Seconds = FMath::FloorToInt ( FMath::Fmod ( RemainingTime , 60.0f ) );

		FString TimeString = FString::Printf ( TEXT ( "%02d:%02d" ) , Minutes , Seconds );
		TimerText->SetText ( FText::FromString ( TimeString ) );
	}
}
