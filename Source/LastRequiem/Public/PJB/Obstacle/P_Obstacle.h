#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

#include "P_Obstacle.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnObstacleBrokenDelegate , AActor* , InActor );

UCLASS()
class LASTREQUIEM_API AP_Obstacle : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	AP_Obstacle();

	UFUNCTION ( BlueprintCallable , Category = "Obstacle" )
	void TestBreak () { OnBroken (); };

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
	FOnObstacleBrokenDelegate OnObstacleBrokenDelegate;

protected:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<class UCapsuleComponent> CollisionComp = nullptr;
	// 포인트
	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Sprite")
	TObjectPtr<class UIndicatorSpriteComponent> SpriteComp = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<UGeometryCollectionComponent> GeometryComp = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Health" )
	float MaxHealth = 300.0f;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Health" )
	float Health = 300.0f;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Range" )
	float AggroRange = 500.0f;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Config = "Data|Destruction" )
	TSubclassOf<AActor> MasterFieldClass = nullptr;

private:
	bool bIsBroken = false;
};
