// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "K_Drone.generated.h"



UCLASS()
class LASTREQUIEM_API AK_Drone : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AK_Drone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void InitializeComponents();
	void UpdateDroneSpeed(float DeltaTime);
	void UpdateDroneMovement(float DeltaTime);
	void UpdateDroneRotation(float DeltaTime);
	
protected:
	
public:
	void Move(const FVector2D& inputValue);
	void UpDown(float inputValue);
	void ChangeMode();
	void UseSkill01();
	void UseSkill02();

	
private:
	//movement 
	FVector2D moveInputValue = FVector2D::ZeroVector;
	float upDownInputValue = 0.0f;
	const float DRONE_MASS_WEIGHT = 5.0f;

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
	
	//Data
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Data")
	TObjectPtr<class UK_DroneData> droneData;
	
	////UI
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	// TSubclassOf<class UUserWidget> droneUIFactory;
	//
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|UI")
	// TObjectPtr<class UUserWidget> droneUI;
	//
	////SFX
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Sound")
	// TObjectPtr<class USoundBase> droneSound;
	
};
