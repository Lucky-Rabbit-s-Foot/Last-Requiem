// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BJM/Unit/B_UnitBase.h"
#include "B_UnitGrenadier.generated.h"


class UNiagaraSystem;

UCLASS()
class LASTREQUIEM_API AB_UnitGrenadier : public AB_UnitBase
{
	GENERATED_BODY()
	
public:
	AB_UnitGrenadier ();

protected:
	virtual void OnAttackHit_Unit () override;

public:
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Combat" )
	float ExplosionRadius = 200.0f;

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Unit|Combat" )
	UNiagaraSystem* ExplosionEffect;
};
