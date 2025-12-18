#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "P_GameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnChangeScore , int32 , NewScore );

UCLASS()
class LASTREQUIEM_API AP_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AP_GameStateBase ();

	FOnChangeScore OnChangeScore;

	void RegisterFortress ( AActor* InFortress ) { Fortress = InFortress; }
	AActor* GetFortress () const { return Fortress.Get (); }

	UFUNCTION ( BlueprintCallable , Category = "Data|Score" )
	void AddScore ( int32 Amount );
	UFUNCTION ( BlueprintPure , Category = "Data|Score" )
	int32 GetScore () const { return TotalScore; }

protected:
	UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = "Data|Score" )
	int32 TotalScore = 0;

private:
	TWeakObjectPtr<AActor> Fortress;
};
