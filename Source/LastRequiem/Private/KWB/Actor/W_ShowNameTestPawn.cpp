// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/Actor/W_ShowNameTestPawn.h"

// Sets default values
AW_ShowNameTestPawn::AW_ShowNameTestPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AW_ShowNameTestPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AW_ShowNameTestPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AW_ShowNameTestPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

