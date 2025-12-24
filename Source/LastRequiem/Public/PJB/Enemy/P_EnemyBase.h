#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"

#include "P_EnemyBase.generated.h"

UENUM (BlueprintType)
enum class EP_EnemyType : uint8
{
	Melee		UMETA ( DisplayName = "Melee" ) ,
	Range		UMETA ( DisplayName = "Range" ) ,
	EnhancedMelee	UMETA ( DisplayName = "EnhancedMelee" ) ,
	EnhancedRange	UMETA ( DisplayName = "EnhancedRange" )
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDieDelegate, int32, InID);

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
	void BindDrone ( class AK_Drone* InDrone );

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
	
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Data|Weapon" )
	TObjectPtr<class USkeletalMeshComponent> WeaponMeshComp = nullptr;
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly, Category = "Data|Weapon")
	TObjectPtr<class UNiagaraComponent> MuzzleFlashComp = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|ID" )
	int32 EnemyID = 0;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Movement" )
	float BaseMoveSpeed = 100.0f;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Movement" )
	float CombatMoveSpeed = 100.0f;
	
	// Cached combat data
	float CachedAttackRange = 100.0f;
	UPROPERTY ()
	UAnimMontage* CachedAttackMontage = nullptr;

	bool bIsCombat = false;
	bool bIsAlive = true;
};
