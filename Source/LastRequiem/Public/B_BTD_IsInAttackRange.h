// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "B_BTD_IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UB_BTD_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UB_BTD_IsInAttackRange ();

protected:
	// 조건을 검사해서 true/false를 반환하는 함수
	virtual bool CalculateRawConditionValue ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory ) const override;
};
