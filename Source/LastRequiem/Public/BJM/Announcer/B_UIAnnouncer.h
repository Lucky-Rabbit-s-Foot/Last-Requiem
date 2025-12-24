// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "B_UIAnnouncer.generated.h"

class USoundBase;
class AP_Fortress;

UCLASS()
class LASTREQUIEM_API AB_UIAnnouncer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AB_UIAnnouncer();

	UFUNCTION ( BlueprintCallable , Category = "Announcer" )
	void PlayUnitDeathSound ( FString UnitName );

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY ( EditAnywhere , Category = "Announcer|Start" )
	USoundBase* GameStart_01;

	UPROPERTY ( EditAnywhere , Category = "Announcer|Start" )
	USoundBase* GameStart_02; 

	UPROPERTY ( EditAnywhere , Category = "Announcer|Start" )
	float GameStart_Delay = 3.0f; 

	
	UPROPERTY ( EditAnywhere , Category = "Announcer|Wave" )
	USoundBase* WaveStartSound;

	UPROPERTY ( EditAnywhere , Category = "Announcer|Wave" )
	USoundBase* WaveEndSound; 


	UPROPERTY ( EditAnywhere , Category = "Announcer|Core" )
	USoundBase* CoreHP_50;

	UPROPERTY ( EditAnywhere , Category = "Announcer|Core" )
	USoundBase* CoreHP_25;

	UPROPERTY ( EditAnywhere , Category = "Announcer|Core" )
	USoundBase* CoreHP_10;

	
	UPROPERTY ( EditAnywhere , Category = "Announcer|End" )
	USoundBase* GameOver_CoreDestroyed;


	UPROPERTY ( EditAnywhere , Category = "Announcer|Unit" )
	TMap<FString , USoundBase*> UnitDeathSoundMap;



	void PlayGameStart_Sequence2 ();

	UFUNCTION ()
	void HandleGameStart ();

	UFUNCTION ()
	void HandleWaveStart ();

	UFUNCTION ()
	void HandleWaveEnd ();

	UFUNCTION ()
	void HandleCoreDamaged ( AP_Fortress* Fortress );

	UFUNCTION ()
	void HandleCoreBroken ( AP_Fortress* Fortress );

	void PlayVoice ( USoundBase* Sound );

private:

	bool bPlayed50;
	bool bPlayed25;
	bool bPlayed10;
	bool bPlayed0;

};
