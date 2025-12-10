// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "B_UnitAIController.h"

#include "B_UnitBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE ( FOnUnitDieDelegate );

class UB_UnitStatusComponent;


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

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "Unit|Component" )
	UB_UnitStatusComponent* StatusComponent;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Data|Gameplay Tag" )
	FGameplayTagContainer GameplayTags;

	// 위젯 찾으려고 타이머 설정한거(위젯찾으면 사라짐)
	FTimerHandle FindWidgetTimerHandle;

	// 위젯 찾게 돌리는거
	UFUNCTION ()
	void FindMapWidgetLoop ();

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Data" )
	UTexture2D* MyProfileImage = nullptr;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Data" )
	FText MyUnitName;

	// 유닛 프로필 데이터
	UFUNCTION ( BlueprintCallable , Category = "Unit|Data" )
	FUnitProfileData GetUnitProfileData ();


public:

	// 이동시 멘탈 체크
	UFUNCTION(BlueprintCallable, Category = "Unit|Move")
	void UnitMentalCheck_Move(float InX, float InY);

	UFUNCTION(BlueprintCallable, Category = "Unit|Move")
	void ProcessMoveCommand(float InX, float InY);


	UFUNCTION ()
	void CommandMoveToLocation ( FVector TargetLocation );


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Attack")
	float AttackRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Attack")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit|Attack")
	FName MuzzleSocketName = TEXT("Muzzle");

	virtual float TakeDamage ( float DamageAmount , struct FDamageEvent const& DamageEvent , class AController* EventInstigator , AActor* DamageCauser ) override;

	UFUNCTION(BlueprintCallable, Category = "Unit|Attack")
	void UnitAttack(AActor* TargetActor);

private:
	bool bIsAlive = true;
	FOnUnitDieDelegate OnUnitDieDelegate;

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

};
