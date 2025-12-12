// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "B_BTT_Stop.generated.h"

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UB_BTT_Stop : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UB_BTT_Stop ();

	virtual EBTNodeResult::Type ExecuteTask ( UBehaviorTreeComponent& OwnerComp , uint8* NodeMemory ) override;

};
