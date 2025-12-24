// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "B_UnitAIController.h"
#include "NiagaraSystem.h"
#include "Delegates/Delegate.h"

#include "B_UnitBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnUnitDieDelegate, AActor*, InUnit );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnCombatStateChangedDelegate , bool , bIsCombat );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams ( FOnUnitSpeak , AActor* , InSpeaker , bool , bIsSpeaking );

class UB_UnitStatusComponent;
class UIndicatorSpriteComponent;
class UAudioComponent;
class UWidgetComponent;
class UW_ShowNameWidget;
class AK_Drone;

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
	virtual void EndPlay ( const EEndPlayReason::Type EndPlayReason ) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY ( VisibleAnywhere , BlueprintReadWrite , Category = "Unit|Visual" )
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Combat|VFX" )
	UNiagaraComponent* NiagaraComp;

	UPROPERTY ( VisibleAnywhere , BlueprintReadWrite , Category = "Unit|Visual" )
	class USpotLightComponent* GunFlashlight;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Unit|Component" )
	UB_UnitStatusComponent* StatusComponent;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Unit|Component" )
	UIndicatorSpriteComponent* IndicatorSprite = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	// 위젯 찾으려고 타이머 설정한거(위젯찾으면 사라짐)
	//FTimerHandle FindWidgetTimerHandle;

	// 위젯 찾게 돌리는거
	//UFUNCTION ()
	//void FindMapWidgetLoop ();

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

	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly , Category = "Unit|UI" )
	TSubclassOf<UW_ShowNameWidget> ShowNameWidgetClass;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Unit|UI" )
	UWidgetComponent* ShowNameWidgetComponent = nullptr;

	UFUNCTION ( BlueprintCallable , Category = "Unit|Data" )
	FUnitProfileData GetUnitProfileData ();


	FUnitProfileData UnitUpdateData;
	

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Setting" )
	EUnitType UnitType;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Setting" )
	UDataTable* UnitDataTable;

	virtual void OnConstruction ( const FTransform& Transform ) override;

	void UnitDataUpdate ();
protected:

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

	UPROPERTY ( VisibleAnywhere , BlueprintReadWrite , Category = "Unit|State" )
	bool bIsSpeaking = false;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Data" )
	FVector FortressLocation;

	UFUNCTION ( BlueprintCallable , Category = "Unit|Logic" )
	void SetSpeakingState ( bool bNewState );

public:
	UFUNCTION ( BlueprintCallable , Category = "Unit|Support" )
	void ReceiveSupport_HP ( float InValue );

	UFUNCTION ( BlueprintCallable , Category = "Unit|Support" )
	void ReceiveSupport_Sanity ( float InValue );

protected:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Stats" )
	float HPRecoveryRatio = 0.25f;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Stats" )
	float SanityRecoveryRatio = 0.3f;


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

	UFUNCTION(BlueprintCallable, Category = "Unit|Attack")
	virtual void OnAttackHit_Unit ();

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Animation" )
	UAnimMontage* AttackMontage;

protected:
	UPROPERTY ()
	AActor* CurrentAttackTarget;

public:

	FOnUnitDieDelegate OnUnitDieDelegate;

	UPROPERTY ( BlueprintAssignable , Category = "Unit|Event" )
	FOnCombatStateChangedDelegate OnCombatStateChanged;

	//UPROPERTY ( BlueprintAssignable , Category = "Unit|Event" )
	//FOnUnitSpeak OnUnitSpeak;

	UPROPERTY ( BlueprintAssignable , Category = "Unit|Event" )
	FOnUnitSpeak OnUnitSpeakDelegate;
	
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

	UFUNCTION ()
	void OnCombatStateChanged_Wrapper ( bool bInCombat );

public:
	void SetSelectedSprite ( bool bIsSelected );

protected:
	FTimerHandle SpeakingTimerHandle;

	void InitializeShowNameWidget ();
	void BindDroneDetection ();
	void UnbindDroneDetection ();
	void HandleDroneUnitDetected ( AActor* DetectedActor );
	void HandleDroneUnitLost ( AActor* LostActor );

	UPROPERTY ( Transient )
	TObjectPtr<UW_ShowNameWidget> ShowNameWidget = nullptr;

	TWeakObjectPtr<AK_Drone> BoundDrone;
	FDelegateHandle UnitDetectedHandle;
	FDelegateHandle UnitLostHandle;

public:
	UFUNCTION ( BlueprintCallable , Category = "Unit|Action" )
	void PlayUnitVoice ( USoundBase* InVoiceSound );

	void StopUnitVoice ();

protected:
	UPROPERTY ( EditAnywhere , BlueprintReadOnly , Category = "Unit|Sound Data" )
	TMap<EUnitBehaviorState , FUnitVoiceProfile> UnitVoiceData;

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Command" )
	USoundBase* Cmd_MoveSound;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Command" )
	USoundBase* Cmd_StopSound;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Command" )
	USoundBase* Cmd_AttackSound;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Command" )
	USoundBase* Cmd_RetreatSound;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Command" )
	USoundBase* Cmd_HoldSound;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Command" )
	USoundBase* Cmd_DisobeySound;

public:
	UFUNCTION ( BlueprintCallable , Category = "Unit|Sound" )
	void PlayVoiceForEvent ( EUnitVoiceEvent InEvent );

protected:
	FTimerHandle TimerHandle_CombatVoiceCooldown;

	bool bCanPlayCombatVoice = true;

	void ResetCombatVoiceCooldown ();

protected:
	void PlayCommandSound ( USoundBase* InSound );

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Setting" )
	USoundAttenuation* UnitVoiceAttenuation;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Setting" )
	USoundConcurrency* UnitVoiceConcurrency;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Setting" )
	USoundClass* UnitSoundClass;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Sound Setting" )
	USoundClass* UnitCommandSoundClass;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Sound" )
	USoundBase* WeaponFireSound;

	UFUNCTION ( BlueprintCallable )
	void PlayWeaponFireSound ();

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Combat|VFX" )
	UNiagaraSystem* MuzzleFlashVFX;

	UFUNCTION ( BlueprintCallable , Category = "Combat" )
	void PlayMuzzleEffect ();

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Sound" )
	USoundBase* FootstepSound;

	UFUNCTION ( BlueprintCallable , Category = "Sound" )
	void PlayFootstepSound ();

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Combat|VFX" )
	UNiagaraSystem* HealingHP;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Combat|VFX" )
	UNiagaraSystem* HealingSanity;

	void PlayHealingHPEffect ();

	void PlayHealingSanityEffect ();

private:
	bool bIsSelectedByPlayer = false;

	void RefreshIndicatorState ();

};
