#include "PJB/ScoreBoard/P_ScoreBlock.h"

#include "Components/TextBlock.h"

void UP_ScoreBlock::SetBlockData ( FText InTitle , int32 InCount , int32 InScore )
{
	if (Title)
	{
		Title->SetText ( InTitle );
	}
	if (Count)
	{
		Count->SetText ( FText::AsNumber ( InCount ) );
	}
	if (Score)
	{
		Score->SetText ( FText::AsNumber ( InScore ) );
	}
}
