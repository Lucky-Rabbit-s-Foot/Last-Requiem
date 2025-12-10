#include "PJB/Enemy/P_AnimInstanceEnemyBase.h"

#include "GameFramework/PawnMovementComponent.h"

void UP_AnimInstanceEnemyBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* OwnerPawn = TryGetPawnOwner();
	if (OwnerPawn)
	{
		OwnerMovementComp = OwnerPawn->GetMovementComponent();
	}
}

void UP_AnimInstanceEnemyBase::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerMovementComp.IsValid())
	{
		Speed = OwnerMovementComp.Get()->Velocity.Size();
	}
}
