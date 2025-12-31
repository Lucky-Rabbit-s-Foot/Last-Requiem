#include "PJB/Copyright/P_CopyrightItemWidget.h"

#include "Components/TextBlock.h"

void UP_CopyrightItemWidget::SetCopyrightData ( const FCopyrightInfo& Info )
{
	if (AssetName) AssetName->SetText ( FText::FromString ( Info.AssetName ) );
	if (License) License->SetText ( FText::FromString ( Info.License ) );
	if (Author) Author->SetText ( FText::FromString ( Info.Author ) );
}
