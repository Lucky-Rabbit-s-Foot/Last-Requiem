#include "PJB/Obstacle/P_DetectedObstacleWidget.h"

void UP_DetectedObstacleWidget::NativeConstruct ()
{
	Super::NativeConstruct ();

	SetVisibility ( ESlateVisibility::Hidden );
}

void UP_DetectedObstacleWidget::PlayShowAnimation ()
{
	if (!AppearAnim)
	{
		SetVisibility ( ESlateVisibility::Visible );
		return;
	}
	PlayAnimation ( AppearAnim , 0.0f , 1 , EUMGSequencePlayMode::Forward , 1.0f );
}

void UP_DetectedObstacleWidget::PlayHideAnimation ()
{
	if (!AppearAnim)
	{
		SetVisibility ( ESlateVisibility::Hidden );
		return;
	}
	PlayAnimation ( AppearAnim , 0.0f , 1 , EUMGSequencePlayMode::Reverse , 1.0f );
}
