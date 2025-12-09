#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "P_CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LASTREQUIEM_API UP_CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UP_CombatComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	inline bool IsCombat() { return bIsCombat; }

	void TakeDamage(float InDamage);
	void OnAttack(AActor* Target);
	void OnDie();

	inline float GetMaxHealth() { return MaxHealth; }
	inline float GetHealth() { return Health; }
	inline float GetAttackRange() { return AttackRange; }
	inline float GetAttackPower() { return AttackPower; }
	inline float GetTerrorValue() { return TerrorValue; }
	inline bool IsAlive() { return bIsAlive; }

private:
	void InitPrimitiveValues();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Health")
	float MaxHealth = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data|Health")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Combat")
	float AttackRange = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Combat")
	float AttackPower = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Terror")
	float TerrorValue = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Combat")
	bool bIsCombat = false;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Combat" )
	bool bIsAlive = true;
};
