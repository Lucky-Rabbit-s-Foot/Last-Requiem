#include "KWB/Component/W_SelectedSpriteComponent.h"
#include "KWB/UI/W_MapWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UW_SelectedSpriteComponent::UW_SelectedSpriteComponent ()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	SetGenerateOverlapEvents ( false );
}

void UW_SelectedSpriteComponent::BeginPlay ()
{
	Super::BeginPlay ();

	DefaultSprite = GetSprite ();

	// 기본은 안 보이게 시작
	SetSelectedVisible ( false );
	bIsUnitSelected = false;
}

void UW_SelectedSpriteComponent::EndPlay ( const EEndPlayReason::Type EndPlayReason )
{
	if (CachedMapWidget.IsValid ())
	{
		CachedMapWidget->OnMapUnitSelected.RemoveDynamic ( this , &UW_SelectedSpriteComponent::HandleMapUnitSelected );
	}

	Super::EndPlay ( EndPlayReason );
}

void UW_SelectedSpriteComponent::TickComponent ( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent ( DeltaTime , TickType , ThisTickFunction );

	if (!bBoundToMapWidget)
	{
		TryBindToMapWidget ();

		if (bBoundToMapWidget)
		{
			SetComponentTickEnabled ( false );
		}
	}
}

void UW_SelectedSpriteComponent::TryBindToMapWidget ()
{
	if (bBoundToMapWidget) return;

	UWorld* World = GetWorld ();
	if (!World) return;

	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass ( World , FoundWidgets , UW_MapWidget::StaticClass () , false );

	for (UUserWidget* Widget : FoundWidgets)
	{
		UW_MapWidget* MapWidget = Cast<UW_MapWidget> ( Widget );
		if (!MapWidget) continue;

		if (!MapWidget->OnMapUnitSelected.IsAlreadyBound ( this , &UW_SelectedSpriteComponent::HandleMapUnitSelected ))
		{
			MapWidget->OnMapUnitSelected.AddDynamic ( this , &UW_SelectedSpriteComponent::HandleMapUnitSelected );
		}

		CachedMapWidget = MapWidget;
		bBoundToMapWidget = true;

		UE_LOG ( LogTemp , Log , TEXT ( "[SelectedSpriteComponent] Bound to MapWidget (%s)" ) , *MapWidget->GetName () );
		break;
	}
}

void UW_SelectedSpriteComponent::HandleMapUnitSelected ( AActor* SelectedActor )
{
	ApplySelectionVisual ( SelectedActor );
}

void UW_SelectedSpriteComponent::ApplySelectionVisual ( AActor* SelectedActor )
{
	AActor* OwnerActor = GetOwner ();
	if (!OwnerActor) return;

	const bool bNowSelected = (SelectedActor != nullptr && SelectedActor == OwnerActor);

	if (bNowSelected != bIsUnitSelected)
	{
		if (bNowSelected)
		{
			PlaySelectSound ();
		}
	}

	bIsUnitSelected = bNowSelected;

	SetSelectedVisible ( bIsUnitSelected );

}

void UW_SelectedSpriteComponent::SetSelectedVisible ( bool bShouldBeVisible )
{
	SetVisibility ( bShouldBeVisible , true );
	SetHiddenInGame ( !bShouldBeVisible );
}

void UW_SelectedSpriteComponent::PlaySelectSound ()
{
	UWorld* World = GetWorld ();
	if (!World) return;

	if (SelectSound2D)
	{
		UGameplayStatics::PlaySound2D ( World , SelectSound2D , SelectVolume , SelectPitch );
		return;
	}
}