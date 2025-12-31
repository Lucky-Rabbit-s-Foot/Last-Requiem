#include "PJB/Copyright/P_CopyrightListWidget.h"

#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "PJB/Copyright/P_CopyrightItemWidget.h"
#include "PJB/Data/P_CopyrightData.h"

void UP_CopyrightListWidget::NativeConstruct ()
{
	Super::NativeConstruct ();
	SetVisibility ( ESlateVisibility::Hidden );
}

void UP_CopyrightListWidget::InitCopyrightList ( UP_CopyrightData* DA )
{
	if (!DA || !CopyrightScrollBox || !ItemWidgetClass) return;

	if (IsInitialized) return;
	IsInitialized = true;

	CopyrightScrollBox->ClearChildren ();

	for (const FCopyrightInfo& Info : DA->CopyrightList)
	{
		UP_CopyrightItemWidget* Item = CreateWidget<UP_CopyrightItemWidget> ( this , ItemWidgetClass );
		if (Item)
		{
			Item->SetCopyrightData ( Info );
			UPanelSlot* PanelSlot = CopyrightScrollBox->AddChild ( Item );

			UScrollBoxSlot* ScrollSlot = Cast<UScrollBoxSlot> ( PanelSlot );

			if (ScrollSlot)
			{
				ScrollSlot->SetPadding ( FMargin ( 0.0f , 0.0f , 0.0f , 20.0f ) );
				ScrollSlot->SetHorizontalAlignment ( EHorizontalAlignment::HAlign_Fill );
			}
		}
	}
}
