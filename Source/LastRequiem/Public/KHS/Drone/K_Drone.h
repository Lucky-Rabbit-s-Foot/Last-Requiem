// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "K_Drone.generated.h"

//탐지용 델리게이트
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitDetected, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitLostDetection, AActor*);


UCLASS()
class LASTREQUIEM_API AK_Drone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AK_Drone();

	FOnUnitDetected onUnitDetected;
	FOnUnitLostDetection onUnitLostDetection;
	
protected:
	// Called when the game starts or when spawned
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void UpdateDroneRotationByMovement(float DeltaTime);

	// (20251224) P : 드론 충돌 (Start)
	virtual void NotifyHit ( class UPrimitiveComponent* MyComp , AActor* Other , class UPrimitiveComponent* OtherComp , bool bSelfMoved , FVector HitLocation , FVector HitNormal , FVector NormalImpulse , const FHitResult& Hit ) override;
	// (20251224) P : 드론 충돌 (End)


private:
	void InitializeDefaultComponents(); //컴포넌트 초기화 내부 헬퍼 함수
	void UpdateDroneMovement(float DeltaTime);
	void UpdateDroneRotation(float DeltaTime);
	
	//Detection Helper
	void InitializeDetectedUnitSlot(); //게임 시작시 맵 전체 유닛 스캔 헬퍼 함수
	void UpdateDetectedUnitSlot(); //게임 진행 중 유닛 탐지 업데이트 헬퍼 함수
	
	// (20251223) W : 드론 스킬 쿨타임 핸들 용도 멤버 추가 (Start)
	void StartSkill01Cooldown ();
	void StartSkill02Cooldown ();
	void OnSkill01CooldownFinished ();
	void OnSkill02CooldownFinished ();
	// (20251223) W : 드론 스킬 쿨타임 핸들 용도 멤버 추가 (End)

public:
	//Movement
	void Move(const FVector2D& inputValue);
	void UpDown(float inputValue);
	
	//Skill
	void UseSkill01();
	void UseSkill02();

	// (20251223) W : 드론 스킬 쿨타임 핸들 용도 함수 추가 (Start)
	// Cooldown Query (UMG에서 프로그레스바 바인딩용)
	UFUNCTION ( BlueprintPure , Category = "LR|Skill|Cooldown" )
	bool IsSkill01Ready () const;

	UFUNCTION ( BlueprintPure , Category = "LR|Skill|Cooldown" )
	float GetSkill01CooldownRemaining () const;

	// 0~1 (1=쿨다운 시작 직후, 0=사용 가능)
	UFUNCTION ( BlueprintPure , Category = "LR|Skill|Cooldown" )
	float GetSkill01CooldownRatioRemaining () const;

	UFUNCTION ( BlueprintPure , Category = "LR|Skill|Cooldown" )
	bool IsSkill02Ready () const;

	UFUNCTION ( BlueprintPure , Category = "LR|Skill|Cooldown" )
	float GetSkill02CooldownRemaining () const;

	// 0~1 (1=쿨다운 시작 직후, 0=사용 가능)
	UFUNCTION ( BlueprintPure , Category = "LR|Skill|Cooldown" )
	float GetSkill02CooldownRatioRemaining () const;
	// (20251223) W : 드론 스킬 쿨타임 핸들 용도 함수 추가 (End)

private:
	//movement 
	FVector2D moveInputValue = FVector2D::ZeroVector;
	float upDownInputValue = 0.0f;
	const float DRONE_MASS_WEIGHT = 5.0f;
	
	//Detection
	FTimerHandle detectionTimerHandle; 
	TSet<AActor*> previouslyDetectedUnits; //Detected Unit Set
	TSet<AActor*> previouslyDetectedEnemys;

	// (20251223) W : 드론 스킬 쿨타임 핸들 용도 멤버 추가 (Start)
	// 스킬 쿨타임 : 테스트 후 값 결정 필요
	UPROPERTY ( EditDefaultsOnly , Category = "LR|Skill|Cooldown" , meta = (ClampMin = "0.0") )
	float Skill01CooldownDuration = 5.0f;

	UPROPERTY ( EditDefaultsOnly , Category = "LR|Skill|Cooldown" , meta = (ClampMin = "0.0") )
	float Skill02CooldownDuration = 5.0f;

	FTimerHandle Skill01CooldownTimerHandle;
	FTimerHandle Skill02CooldownTimerHandle;
	// (20251223) W : 드론 스킬 쿨타임 핸들 용도 멤버 추가 (End)

	// (20251224) P : 드론 충돌 (Start)
	FTimerHandle DampingRestoreTimer;
	float DefaultLinearDamping = 0.01f;
	// (20251224) P : 드론 충돌 (End)

protected:
	//Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Components")
	TObjectPtr<class USphereComponent> sphereComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Components")
	TObjectPtr<class USkeletalMeshComponent> meshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Components")
	TObjectPtr<class USpringArmComponent> springArmComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Components")
	TObjectPtr<class UCameraComponent> cameraComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Components")
	TObjectPtr<class UIndicatorSpriteComponent> indicatorComp;

	UPROPERTY ( VisibleAnywhere , BlueprintReadOnly , Category = "LR|Components" )
	TObjectPtr<class UAudioComponent> audioComp;

	UPROPERTY ( EditDefaultsOnly , BlueprintReadOnly , Category = "LR|Sound" )
	TObjectPtr<class USoundBase> flightSound;

	//Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Data")
	TObjectPtr<class UK_DroneData> droneData;
	
	//Anim
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Anim")
	TObjectPtr<class UAnimMontage> flightMontage;
	
	//
	////SFX
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Sound")
	// TObjectPtr<class USoundBase> droneSound;
	
};
