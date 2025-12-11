// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/UI/K_UnitListWidget.h"

#include "BJM/Unit/B_UnitBase.h"
#include "Components/VerticalBox.h"
#include "KHS/Drone/K_Drone.h"
#include "Kismet/GameplayStatics.h"

void UK_UnitListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	AK_Drone* drone = Cast<AK_Drone>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (drone)
	{
		DetectionDelegateHandle = drone->onUnitDetected.AddUObject(this, &UK_UnitListWidget::OnUnitDetected);
		LostDetectionDelegateHandle = drone->onUnitLostDetection.AddUObject(this, &UK_UnitListWidget::OnUnitLostDetection);
	}
}

void UK_UnitListWidget::NativeDestruct()
{
	AK_Drone* drone = Cast<AK_Drone>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (drone)
	{
		if (DetectionDelegateHandle.IsValid())
		{
			drone->onUnitDetected.Remove(DetectionDelegateHandle);
		}
		
		if (LostDetectionDelegateHandle.IsValid())
		{
			drone->onUnitLostDetection.Remove(LostDetectionDelegateHandle);
		}
	}
	
	//모든 유닛 Death델리게이트 해제
	for (auto& pair : DeathDeleageMap)
	{
		AB_UnitBase* unit = Cast<AB_UnitBase>(pair.Key);
		if (unit && pair.Value.IsValid())
		{
			//TODO : 나중에 public으로 바뀌면 다시 주석 해제
			//unit->OnUnitDieDelegate.Remove(pair.Value);
		}
	}
	DeathDeleageMap.Empty();
	
	Super::NativeDestruct();
}

void UK_UnitListWidget::OnUnitDetected(AActor* detectedUnit)
{
	if (!detectedUnit)
	{
		return;
	}
	
	AB_UnitBase* unit = Cast<AB_UnitBase>(detectedUnit);
	if (!unit)
	{
		return;
	}
	
	UK_UnitSlotWiddget* unitSlot = GetOrCreateUnitSlot(detectedUnit);
	if (unitSlot)
	{
		//State Update
		if (unitSlot->GetSlotState() != EUnitSlotState::DEAD)
		{
			unitSlot->SetSlotState(EUnitSlotState::DETECTED);
		}
		
		//Date Update
		FUnitProfileData data = unit->GetUnitProfileData();
		unitSlot->UpdateUnitData(data);
	}
}

void UK_UnitListWidget::OnUnitLostDetection(AActor* lostUnit)
{
	if (!lostUnit)
	{
		return;
	}
	
	if (unitSlotMap.Contains(lostUnit))
	{
		UK_UnitSlotWiddget* unitSlot = unitSlotMap[lostUnit];
		if (unitSlot)
		{
			//DEAD가 아니면 UNDETECTED로 변경
			if (unitSlot->GetSlotState() != EUnitSlotState::DEAD)
			{
				unitSlot->SetSlotState(EUnitSlotState::UNDETECTED);
			}
		}
	}
}

UK_UnitSlotWiddget* UK_UnitListWidget::GetOrCreateUnitSlot(AActor* unitActor)
{
	if (!unitActor)
	{
		return nullptr;
	}
	
	//이미 존재하면 반환
	if (unitSlotMap.Contains(unitActor))
	{
		return unitSlotMap[unitActor];
	}
	
	//새로 생성
	if (!unitSlotWidget)
	{
		return nullptr;
	}
	
	UK_UnitSlotWiddget* newSlot = CreateWidget<UK_UnitSlotWiddget>(GetWorld(), unitSlotWidget);
	if (newSlot)
	{
		unitSlotMap.Add(unitActor, newSlot);
		AddUnitSlot(newSlot);
		
		//Death Delegate Subscribe
		AB_UnitBase* unit = Cast<AB_UnitBase>(unitActor);
		if (unit)
		{
			//TODO : Delegate 접근지정 해제후에 주석해제
			// FDelegateHandle DeathHandle = unit->OnUnitDieDelegate.AddLambda([this], unitActor()
			// {
			// 	//Map 존재 확인
			// 	if (unitSlotMap.Contains(unitActor))
			// 	{
			// 		UK_UnitSlotWiddget* unitSlot = unitSlotMap[unitActor];
			// 		if (unitSlot)
			// 		{
			// 			unitSlot->SetSlotState(EUnitSlotState::DEAD);
			// 		}
			// 	}
			// 	
			// 	if (DeathDeleageMap.Contains(unitActor))
			// 	{
			// 		unit->OnUnitDieDelegate.Remove(DeathDeleageMap[unitActor]);
			// 		DeathDeleageMap.Remove(unitActor);
			// 	}
			// 	
			// 	DeathDeleageMap.Add(unitActor, DeathHandle);
			// });
		}
	}
	
	return newSlot;
}

void UK_UnitListWidget::AddUnitSlot(UK_UnitSlotWiddget* newSlot)
{
	if (!newSlot || !UnitSlotList)
	{
		return;
	}
	
	UnitSlotList->AddChildToVerticalBox(newSlot);
}
