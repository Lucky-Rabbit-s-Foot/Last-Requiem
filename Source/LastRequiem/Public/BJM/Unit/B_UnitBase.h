// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "B_UnitAIController.h"
#include "B_UnitBase.generated.h"

class UB_UnitStatusComponent;


UCLASS()
class LASTREQUIEM_API AB_UnitBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AB_UnitBase();

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

public:

	// 좌표를 받아 이동하는 함수
	UFUNCTION(BlueprintCallable, Category = "Unit|Command")
	void CommandMoveToLocation(float InX, float InY);

};
