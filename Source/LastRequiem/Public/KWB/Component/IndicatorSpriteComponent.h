// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "IndicatorSpriteComponent.generated.h"

class UPaperSprite;
class UMaterialInstanceDynamic;

/**
 * 
 */
UCLASS( Blueprintable , ClassGroup = (Custom) , meta = (BlueprintSpawnableComponent) )
class LASTREQUIEM_API UIndicatorSpriteComponent : public UPaperSpriteComponent
{
	GENERATED_BODY()
	
public:
	UIndicatorSpriteComponent ();

	virtual void BeginPlay () override;
	virtual void TickComponent ( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction ) override;

protected:
	UPROPERTY ( EditAnywhere , Category = "Data|Spirte" )
	TObjectPtr<UPaperSprite> SpirteNormal = nullptr;

	UPROPERTY ( EditAnywhere , Category = "Data|Spirte" )
	TObjectPtr<UPaperSprite> SpirteCombat = nullptr;

	UPROPERTY ( EditAnywhere , Category = "Data|Spirte" )
	TObjectPtr<UPaperSprite> SpirteDead = nullptr;

	// Glow 세팅
	UPROPERTY ( EditAnywhere , Category = "Data|Glow" )
	float GlowSpeed;

	UPROPERTY ( EditAnywhere , Category = "Data|Glow" )
	float GlowMin;

	UPROPERTY ( EditAnywhere , Category = "Data|Glow" )
	float GlowMax;

private:
	// Glow용
	UPROPERTY ( Transient )
	UMaterialInstanceDynamic* DynamicMID;

	bool  bGlowActive;
	float GlowTime;

	void UpdateSpriteForState ();
	void UpdateGlow ( float DeltaTime );

	FTimerHandle GlowTimer;	// Unit이 적 발견 시 반짝이는 효과 일정 시간 유지용
};
