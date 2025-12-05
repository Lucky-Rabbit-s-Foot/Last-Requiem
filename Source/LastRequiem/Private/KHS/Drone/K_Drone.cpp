// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/Drone/K_Drone.h"

// Sets default values
AK_Drone::AK_Drone()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AK_Drone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AK_Drone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AK_Drone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

