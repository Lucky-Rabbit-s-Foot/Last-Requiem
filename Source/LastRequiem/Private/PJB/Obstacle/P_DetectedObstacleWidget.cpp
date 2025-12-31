#include "PJB/Obstacle/P_DetectedObstacleWidget.h"

void UP_DetectedObstacleWidget::NativeConstruct ()
{
	Super::NativeConstruct ();

	SetVisibility ( ESlateVisibility::Hidden );
}

void UP_DetectedObstacleWidget::PlayShowAnimation ()
{
	if (!AppearAnim) return;
	
	SetVisibility ( ESlateVisibility::Visible );

	FWidgetAnimationDynamicEvent AnimFinishedDelegate;
	AnimFinishedDelegate.BindDynamic ( this , &UP_DetectedObstacleWidget::OnHideAnimFinished );
	UnbindFromAnimationFinished ( AppearAnim , AnimFinishedDelegate ); 
	
	PlayAnimation ( AppearAnim , 0.0f , 1 , EUMGSequencePlayMode::Forward , 1.0f );
}

void UP_DetectedObstacleWidget::PlayHideAnimation ()
{
	if (!AppearAnim) return;

	FWidgetAnimationDynamicEvent AnimFinishedDelegate;
	AnimFinishedDelegate.BindDynamic ( this , &UP_DetectedObstacleWidget::OnHideAnimFinished );
	UnbindFromAnimationFinished ( AppearAnim , AnimFinishedDelegate );

	PlayAnimation ( AppearAnim , 0.0f , 1 , EUMGSequencePlayMode::Reverse );

	BindToAnimationFinished ( AppearAnim , AnimFinishedDelegate );
}

void UP_DetectedObstacleWidget::OnHideAnimFinished ()
{
	SetVisibility ( ESlateVisibility::Hidden );
	
	FWidgetAnimationDynamicEvent AnimFinishedDelegate;
	AnimFinishedDelegate.BindDynamic ( this , &UP_DetectedObstacleWidget::OnHideAnimFinished );
	UnbindFromAnimationFinished ( AppearAnim , AnimFinishedDelegate );
}
