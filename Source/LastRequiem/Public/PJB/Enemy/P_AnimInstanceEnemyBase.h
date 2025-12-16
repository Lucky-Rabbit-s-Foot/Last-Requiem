#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "P_AnimInstanceEnemyBase.generated.h"

UCLASS()
class LASTREQUIEM_API UP_AnimInstanceEnemyBase : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	void SetCombatState ( bool bNewIsCombat ) { bIsCombat = bNewIsCombat; }
	void SetDeadState ( bool bNewIsDead ) { bIsDead = bNewIsDead; }
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Speed")
	float Speed = 0.0f;

	UPROPERTY ( VisibleAnywhere , BlueprintReadWrite , Category = "Data|State" )
	bool bIsCombat = false;
	UPROPERTY ( VisibleAnywhere , BlueprintReadWrite , Category = "Data|State" )
	bool bIsDead = false;

private:
	TWeakObjectPtr<const UPawnMovementComponent> OwnerMovementComp = nullptr;
};
