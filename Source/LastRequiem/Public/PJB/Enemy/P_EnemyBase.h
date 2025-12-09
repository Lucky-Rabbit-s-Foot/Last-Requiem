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

public:
	inline bool IsAlive () { return bIsAlive; }

	UFUNCTION()
	void OnTakeDamage (
		AActor* DamagedActor ,
		float Damage ,
		const class UDamageType* DamageType,
		class AController* InstigateBy ,
		AActor* DamageCauser );

	void OnDie ();

protected:
	// Components
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<class UP_CombatComponent> CombatComp = nullptr;
	
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Speed" )
	float WalkSpeed = 300.0f;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Speed" )
	float SprintSpeed = 600.0f;

private:
	bool bIsAlive = true;

};
