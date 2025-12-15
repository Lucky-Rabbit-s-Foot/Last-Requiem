// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "B_UnitAIController.h"

#include "B_UnitBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnUnitDieDelegate, AActor*, InUnit );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnCombatStateChangedDelegate , bool , bIsCombat );

class UB_UnitStatusComponent;
class UIndicatorSpriteComponent;


UCLASS()
class LASTREQUIEM_API AB_UnitBase : public ACharacter, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AB_UnitBase();

	virtual void GetOwnedGameplayTags ( FGameplayTagContainer& TagContainer ) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY ( VisibleAnywhere , BlueprintReadWrite , Category = "Unit|Visual" )
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY ( VisibleAnywhere , BlueprintReadWrite , Category = "Unit|Visual" )
	class USpotLightComponent* GunFlashlight;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Unit|Component" )
	UB_UnitStatusComponent* StatusComponent;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Unit|Component" )
	UIndicatorSpriteComponent* IndicatorSprite = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	// 위젯 찾으려고 타이머 설정한거(위젯찾으면 사라짐)
	FTimerHandle FindWidgetTimerHandle;

	// 위젯 찾게 돌리는거
	UFUNCTION ()
	void FindMapWidgetLoop ();

protected:
	float OriginalAttackDamage;
	float OriginalAttackRange;

	UFUNCTION ()
	void OnBehaviorStateChanged_Unit ( EUnitBehaviorState NewState );

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Data" )
	UTexture2D* MyProfileImage = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Data" )
	FText MyUnitName;

	UFUNCTION ( BlueprintCallable , Category = "Unit|Data" )
	FUnitProfileData GetUnitProfileData ();


	FUnitProfileData UnitUpdateData;
	

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Setting" )
	EUnitType UnitType;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Setting" )
	UDataTable* UnitDataTable;

	virtual void OnConstruction ( const FTransform& Transform ) override;

protected:
	void UnitDataUpdate ();

public:

	// 이동시 멘탈 체크
	UFUNCTION(BlueprintCallable, Category = "Unit|Move")
	void UnitMentalCheck_Move(float InX, float InY);

	UFUNCTION(BlueprintCallable, Category = "Unit|Move")
	void ProcessMoveCommand(float InX, float InY);


	UFUNCTION ( BlueprintCallable , Category = "Unit|Command" )
	void CommandMoveToLocation ( FVector TargetLocation );

	UFUNCTION ( BlueprintCallable , Category = "Unit|Command" )
	void CommandAttackMove ( FVector TargetLocation );

	UFUNCTION ( BlueprintCallable , Category = "Unit|Command" )
	void CommandStop ();
	
	UFUNCTION ( BlueprintCallable , Category = "Unit|Command" )
	void CommandHold ();

	UFUNCTION ( BlueprintCallable , Category = "Unit|Command" )
	void CommandRetreat ();

	UFUNCTION ()
	void OnAttackButtonClicked_Unit ();  

	UFUNCTION ()
	void OnStopButtonClicked_Unit ();

	UFUNCTION ()
	void OnHoldButtonClicked_Unit ();

	UFUNCTION ()
	void OnRetreatButtonClicked_Unit ();


	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Unit|State" )
	bool bIsAttackMode = false;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Data" )
	FVector FortressLocation;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Attack")
	float AttackRange = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Attack")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Attack")
	FName MuzzleSocketName = TEXT("Muzzle");

	virtual float TakeDamage ( float DamageAmount , struct FDamageEvent const& DamageEvent , class AController* EventInstigator , AActor* DamageCauser ) override;

	UFUNCTION(BlueprintCallable, Category = "Unit|Attack")
	void UnitAttack(AActor* TargetActor);

	FOnUnitDieDelegate OnUnitDieDelegate;

	UPROPERTY ( BlueprintAssignable , Category = "Unit|Event" )
	FOnCombatStateChangedDelegate OnCombatStateChanged;

	
	UFUNCTION ( BlueprintCallable , Category = "Unit|State" )
	void SetCombatState_Unit ( bool bInCombat );

private:
	bool bIsAlive = true;

public:
	UFUNCTION ( BlueprintCallable )
	inline bool IsAlive () { return bIsAlive; }

	UFUNCTION ()
	void OnTakeDamage_Unit (
		AActor* DamagedActor ,
		float Damage ,
		const class UDamageType* DamageType ,
		class AController* InstigateBy ,
		AActor* DamageCauser
	);

	void OnDie_Unit ();

protected:

	UFUNCTION ()
	void OnHPChanged_Wrapper ( float InCurrentHP , float InMaxHP );
	
	UFUNCTION ()
	void OnSanityChanged_Wrapper ( float InCurrentSanity , float InMaxSanity );


};
