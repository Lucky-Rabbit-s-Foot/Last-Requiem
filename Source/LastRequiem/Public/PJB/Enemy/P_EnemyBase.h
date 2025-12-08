#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "P_EnemyBase.generated.h"

UCLASS()
class LASTREQUIEM_API AP_EnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AP_EnemyBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
