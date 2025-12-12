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

	void InitSpriteComponent ();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const override;

public:
	UFUNCTION(BlueprintCallable)
	inline bool IsAlive() { return bIsAlive; }

	UFUNCTION()
	void OnTakeDamage(
		AActor* DamagedActor,
		float Damage,
		const class UDamageType* DamageType,
		class AController* InstigateBy,
		AActor* DamageCauser
	);

	void OnDie();

	void OnDeactivate ();

private:
	void InitAnimInstance ();

public:
	FOnEnemyDieDelegate OnEnemyDieDelegate;

protected:
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data|Components")
	TObjectPtr<class UP_CombatComponent> CombatComp = nullptr;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Sprite" )
	TObjectPtr<class UPaperSpriteComponent> SpriteComp = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Speed")
	float WalkSpeed = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data|Speed")
	float SprintSpeed = 600.0f;

private:
	UPROPERTY()
	TWeakObjectPtr<UAnimInstance> AnimInstance = nullptr;

	bool bIsAlive = true;
};
