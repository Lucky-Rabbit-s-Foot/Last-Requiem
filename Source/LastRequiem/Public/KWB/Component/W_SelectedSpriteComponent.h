#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "Sound/SoundBase.h"
#include "W_SelectedSpriteComponent.generated.h"

class UW_MapWidget;

UCLASS ()
class LASTREQUIEM_API UW_SelectedSpriteComponent : public UPaperSpriteComponent
{
	GENERATED_BODY ()

public:
	UW_SelectedSpriteComponent ();

	virtual void BeginPlay () override;
	virtual void EndPlay ( const EEndPlayReason::Type EndPlayReason ) override;
	virtual void TickComponent ( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction ) override;

	/** UW_MapWidget::OnMapUnitSelected에 바인딩될 핸들러 */
	UFUNCTION ()
	void HandleMapUnitSelected ( AActor* SelectedActor );

protected:
	UPROPERTY ( Transient )
	TObjectPtr<UPaperSprite> DefaultSprite = nullptr;

	/** 유닛이 "선택되는 순간" 재생할 사운드 (UI 느낌) */
	UPROPERTY ( EditAnywhere , Category = "Audio" )
	TObjectPtr<USoundBase> SelectSound2D = nullptr;

	/** 기본 볼륨/피치 */
	UPROPERTY ( EditAnywhere , Category = "Audio" , meta = (ClampMin = "0.0") )
	float SelectVolume = 1.0f;

	UPROPERTY ( EditAnywhere , Category = "Audio" , meta = (ClampMin = "0.1") )
	float SelectPitch = 1.0f;

private:
	void TryBindToMapWidget ();
	void ApplySelectionVisual ( AActor* SelectedActor );

	void SetSelectedVisible ( bool bShouldBeVisible );

	void PlaySelectSound ();

	bool bIsUnitSelected = false;
	bool bBoundToMapWidget = false;
	TWeakObjectPtr<UW_MapWidget> CachedMapWidget;
};
