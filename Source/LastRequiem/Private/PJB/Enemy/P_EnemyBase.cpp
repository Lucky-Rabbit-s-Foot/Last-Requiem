#include "PJB/Enemy/P_EnemyBase.h"

AP_EnemyBase::AP_EnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AP_EnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AP_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AP_EnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

