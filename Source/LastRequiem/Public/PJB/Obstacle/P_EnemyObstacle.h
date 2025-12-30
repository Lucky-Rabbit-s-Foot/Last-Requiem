#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

#include "P_EnemyObstacle.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnEnemyObstacleBrokenDelegate );

UCLASS()
class LASTREQUIEM_API AP_EnemyObstacle : public APawn, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	AP_EnemyObstacle();

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

	void BindDrone ( class AK_Drone* InDrone );
	void OnDetected ( AActor* DetectedActor );
	void OnLostDetection ( AActor* DetectedActor );


public:
	FOnEnemyObstacleBrokenDelegate OnEnemyObstacleBrokenDelegate;

protected:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<UStaticMeshComponent> Mesh = nullptr;
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<class UCapsuleComponent> CollisionComp = nullptr;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Sprite" )
	TObjectPtr<class UIndicatorSpriteComponent> SpriteComp = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Components" )
	TObjectPtr<UGeometryCollectionComponent> GeometryComp = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Destruction" )
	class UNiagaraSystem* DestructionEffect = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|Health" )
	float MaxHealth = 300.0f;
	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Data|Health" )
	float Health = 300.0f;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Config = "Data|Destruction" )
	TSubclassOf<AActor> MasterFieldClass = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Data|UI" )
	TObjectPtr<class UWidgetComponent> DetectedWidget = nullptr;

private:
	bool bIsBroken = false;
};
