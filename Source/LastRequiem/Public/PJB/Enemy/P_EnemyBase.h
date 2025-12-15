#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"

#include "P_EnemyBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDieDelegate, AActor*, InActor);

UCLASS()
class LASTREQUIEM_API AP_EnemyBase : public ACharacter , public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	AP_EnemyBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	inline bool IsAlive() { return bIsAlive; }

	void InitEnemyData ( class UP_EnemyDataAsset* InData );

	UFUNCTION()
	void OnTakeDamage(
		AActor* DamagedActor,
		float Damage,
		const class UDamageType* DamageType,
		class AController* InstigateBy,
		AActor* DamageCauser
	);

	virtual void GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const override;
	
	UFUNCTION ( BlueprintPure , Category = "Combat" )
	float GetAttackRange () const { return CachedAttackRange; }
	UFUNCTION ( BlueprintPure , Category = "Combat" )
	UAnimMontage* GetAttackMontage () const { return CachedAttackMontage; }

private:
	void InitSpriteComponent ();
	void InitGameplayTag ();
	void InitRotationSetting ();

	void OnDie ();
	void OnDeactivate ();

	void AIPathVisualization ();

public:
	FOnEnemyDieDelegate OnEnemyDieDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Components")
	TObjectPtr<class UP_CombatComponent> CombatComp = nullptr;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Sprite" )
	TObjectPtr<class UIndicatorSpriteComponent> SpriteComp = nullptr;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	// Cached combat data
	float CachedAttackRange = 100.0f;
	UPROPERTY ()
	UAnimMontage* CachedAttackMontage = nullptr;

private:
	bool bIsAlive = true;
};
