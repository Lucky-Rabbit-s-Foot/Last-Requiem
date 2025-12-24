#include "PJB/Data/P_GameResultData.h"

#include "PJB/Data/P_ScoringDataAsset.h"

void FP_GameResultData::ResetResult ()
{
	PlayTime = 0;
	Heal = 0;
	Sanity = 0;
	Dest = 0;
	Melee = 0;
	EnMelee = 0;
	Range = 0;
	EnRange = 0;
	TotalScore = 0;
}

void FP_GameResultData::SetCountResult ( const FP_GameResultData& Count )
{
	PlayTime = Count.PlayTime;
	Heal = Count.Heal;
	Sanity = Count.Sanity;
	Dest = Count.Dest;
	Melee = Count.Melee;
	EnMelee = Count.EnMelee;
	Range = Count.Range;
	EnRange = Count.EnRange;
}

void FP_GameResultData::SetScoreResult ( const FP_GameResultData& Count , const UP_ScoringDataAsset* DA )
{
	if (!DA)
	{
		UE_LOG ( LogTemp , Error , TEXT ( "FP_GameResultData::CalculateResult - ScoringDataAsset is null." ) );
		return;
	}

	TotalScore = 0;
	
	PlayTime = Count.PlayTime * DA->PlayTime;
	TotalScore += PlayTime;

	Heal = Count.Heal * DA->Heal;
	TotalScore += Heal;

	Sanity = Count.Sanity * DA->Sanity;
	TotalScore += Sanity;

	Dest = Count.Dest * DA->Dest;
	TotalScore += Dest;

	Melee = Count.Melee * DA->Melee;
	TotalScore += Melee;

	EnMelee = Count.EnMelee * DA->EnMelee;
	TotalScore += EnMelee;

	Range = Count.Range * DA->Range;
	TotalScore += Range;

	EnRange = Count.EnRange * DA->EnRange;
	TotalScore += EnRange;
}
