// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
private:
	void OnDroneLook(const FInputActionValue& value);
	void OnDroneMove(const FInputActionValue& value);
	void OnDroneUpDown(const FInputActionValue& value);
	void OnDroneScan(const FInputActionValue& value);
	void OnDroneModeChange(const FInputActionValue& value);
	
	void UpdateDroneSpeed(float DeltaTime);
	
protected:
	void UpdateDroneAltitude();
	
public:
	
private:
	FVector curVelocity = FVector::ZeroVector;
	FVector targetVelocity = FVector::ZeroVector;
	FVector2D moveInputValue = FVector2D::ZeroVector;
	float upDownInputValue = 0.0f;

protected:
	
	//Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Components")
	TObjectPtr<class USphereComponent> sphereComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Components")
	TObjectPtr<class USkeletalMeshComponent> meshComp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Components")
	TObjectPtr<class UCameraComponent> cameraComp;
	
	//Input
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputMappingContext> IMC_Drone;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_Move;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_Look;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_UpDown;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_Scan;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LR|Input")
	TObjectPtr<class UInputAction> IA_ModeChange;
	
	
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
