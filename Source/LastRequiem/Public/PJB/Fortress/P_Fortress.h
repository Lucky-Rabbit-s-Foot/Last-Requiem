#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
//#include "GeometryCollection/GeometryCollectionComponent.h"

#include "P_Fortress.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnFortressDamagedDelegate );
DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnFortressBrokenDelegate );

UCLASS()
class LASTREQUIEM_API AP_Fortress : public AActor , public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	AP_Fortress();

	virtual void GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const override;

	bool IsBroken () const { return bIsBroken; }
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick ( float DeltaTime ) override;
	
	UFUNCTION ()
	void OnTakeDamage (
		AActor* DamagedActor ,
		float Damage ,
		const class UDamageType* DamageType ,
		class AController* InstigateBy ,
		AActor* DamageCauser
	);
	void OnBroken ();
	void StartDissolve ();
	bool bIsDissolving = false;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Dissolve" )
	class UNiagaraSystem* DustEffect = nullptr;

public:
	FOnFortressDamagedDelegate OnFortressDamagedDelegate;
	FOnFortressBrokenDelegate OnFortressBrokenDelegate;

protected:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
	
	//UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	//TObjectPtr<UGeometryCollectionComponent> GeometryComp = nullptr;
	//UPROPERTY ( EditAnywhere , BlueprintReadWrite , Config = "Data|Destruction" )
	//TSubclassOf<AActor> MasterFieldClass = nullptr;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Sprite" )
	TObjectPtr<class UIndicatorSpriteComponent> SpriteComp = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Health" )
	float MaxHealth = 300.0f;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Health" )
	float Health = 300.0f;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|SpanTime" )
	float LifeSpanTimeAfterBroken = 3.0f;

private:
	bool bIsBroken = false;
	float CurrentDissolveAmount = 0.0f;

public:
	float GetHealthPercent () const { return Health / MaxHealth; }
};
