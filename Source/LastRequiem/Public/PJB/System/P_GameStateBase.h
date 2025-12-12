#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "P_GameStateBase.generated.h"

UCLASS()
class LASTREQUIEM_API AP_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	void RegisterFortress ( AActor* InFortress ) { Fortress = InFortress; }
	AActor* GetFortress () const { return Fortress.Get (); }

private:
	TWeakObjectPtr<AActor> Fortress;
};
