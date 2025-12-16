#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"

#include "P_Fortress.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnFortressDamagedDelegate , AActor* , InActor );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnFortressBrokenDelegate , AActor* , InActor );

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
	UFUNCTION ()
	void OnTakeDamage (
		AActor* DamagedActor ,
		float Damage ,
		const class UDamageType* DamageType ,
		class AController* InstigateBy ,
		AActor* DamageCauser
	);
	void OnBroken ();

public:
	FOnFortressDamagedDelegate OnFortressDamagedDelegate;
	FOnFortressBrokenDelegate OnFortressBrokenDelegate;

protected:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
	
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Health" )
	float MaxHealth = 300.0f;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Health" )
	float Health = 300.0f;

private:
	bool bIsBroken = false;
};
