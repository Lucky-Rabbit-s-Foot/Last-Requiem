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
	void OnAttack ();

	UFUNCTION()
	void OnTakeDamage(
		AActor* DamagedActor,
		float Damage,
		const class UDamageType* DamageType,
		class AController* InstigateBy,
		AActor* DamageCauser
	);

	void OnDetected ( AActor* DetectedActor );
	void OnLostDetection ( AActor* DetectedActor );

	virtual void GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const override;
	
	UFUNCTION ( BlueprintPure , Category = "Combat" )
	float GetAttackRange () const { return CachedAttackRange; }
	UFUNCTION ( BlueprintPure , Category = "Combat" )
	UAnimMontage* GetAttackMontage () const { return CachedAttackMontage; }
	void SetCombatState ( bool bNewIsCombat );

private:
	void InitGameplayTag ();
	void InitRotationSetting ();

	void OnDie ();
	void OnDeactivate ();

public:
	FOnEnemyDieDelegate OnEnemyDieDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Components")
	TObjectPtr<class UP_CombatComponent> CombatComp = nullptr;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Sprite" )
	TObjectPtr<class UIndicatorSpriteComponent> SpriteComp = nullptr;
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Movement" )
	float BaseMoveSpeed = 100.0f;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Movement" )
	float CombatMoveSpeed = 100.0f;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Score" )
	int32 Score = 100;

	// Cached combat data
	float CachedAttackRange = 100.0f;
	UPROPERTY ()
	UAnimMontage* CachedAttackMontage = nullptr;

	bool bIsCombat = false;
	bool bIsAlive = true;
};
