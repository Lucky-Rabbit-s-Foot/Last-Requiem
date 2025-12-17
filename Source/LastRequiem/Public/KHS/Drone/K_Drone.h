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

private:
	void InitializeDefaultComponents(); //컴포넌트 초기화 내부 헬퍼 함수
	void UpdateDroneMovement(float DeltaTime);
	void UpdateDroneRotation(float DeltaTime);
	
	//Detection Helper
	void InitializeDetectedUnitSlot(); //게임 시작시 맵 전체 유닛 스캔 헬퍼 함수
	void UpdateDetectedUnitSlot(); //게임 진행 중 유닛 탐지 업데이트 헬퍼 함수
	
public:
	//Movement
	void Move(const FVector2D& inputValue);
	void UpDown(float inputValue);
	
	//Skill
	void UseSkill01();
	void UseSkill02();

	
private:
	//movement 
	FVector2D moveInputValue = FVector2D::ZeroVector;
	float upDownInputValue = 0.0f;
	const float DRONE_MASS_WEIGHT = 5.0f;
	
	//Detection
	FTimerHandle detectionTimerHandle; 
	TSet<AActor*> previouslyDetectedUnits; //Detected Unit Set
	TSet<AActor*> previouslyDetectedEnemys;

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
