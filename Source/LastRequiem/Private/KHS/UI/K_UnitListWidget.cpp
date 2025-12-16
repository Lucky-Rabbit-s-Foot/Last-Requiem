// Fill out your copyright notice in the Description page of Project Settings.


#include "KHS/UI/K_UnitListWidget.h"

#include "BJM/Unit/B_UnitBase.h"
#include "Components/Spacer.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "KHS/Drone/K_Drone.h"
#include "Kismet/GameplayStatics.h"

void UK_UnitListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	//Drone 델리게이트 구독(MULTICAST)
	AK_Drone* drone = Cast<AK_Drone>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (drone)
	{
		DetectionDelegateHandle = drone->onUnitDetected.AddUObject(
			this, &UK_UnitListWidget::OnUnitDetected);
		LostDetectionDelegateHandle = drone->onUnitLostDetection.AddUObject(
			this, &UK_UnitListWidget::OnUnitLostDetection);
	}
}

void UK_UnitListWidget::NativeDestruct()
{
	//Drone 델리게이트 구독해제
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
	
	//남아있는 유닛들에 대한 Death 델리게이트 구독해제
	for (auto& pair : unitSlotMap)
	{
		UK_UnitSlotWiddget* slot = pair.Value;
		if (slot)
		{
			slot->onSlotClickedDel.RemoveDynamic(this, &UK_UnitListWidget::OnUnitListClicked);
		}
		
		AB_UnitBase* unit = Cast<AB_UnitBase>(pair.Key);
		if (unit)
		{
			unit->OnUnitDieDelegate.RemoveDynamic(this, &UK_UnitListWidget::OnUnitDied);
		}
	}
	
	
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
		
		//Data Update
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

void UK_UnitListWidget::OnUnitDied(AActor* deadUnit)
{
	if (!deadUnit)
	{
		return;
	}
	
	//유닛슬롯 맵핑에서 상태변경
	if (unitSlotMap.Contains(deadUnit))
	{
		UK_UnitSlotWiddget* unitSlot = unitSlotMap[deadUnit];
		if (unitSlot)
		{
			unitSlot->SetSlotState(EUnitSlotState::DEAD);
		}
	}
	
	//구독해제
	AB_UnitBase* unit = Cast<AB_UnitBase>(deadUnit);
	if (unit)
	{
		unit->OnUnitDieDelegate.RemoveDynamic(this, &UK_UnitListWidget::OnUnitDied);
	}
}

void UK_UnitListWidget::OnUnitListClicked(AActor* selectedUnit)
{
	onUnitListSelectedDel.Broadcast(selectedUnit);
}

UK_UnitSlotWiddget* UK_UnitListWidget::GetOrCreateUnitSlot(AActor* unitActor)
{
	if (!unitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("unitActor is null!"));
		return nullptr;
	}
	
	//이미 존재하면 반환
	if (unitSlotMap.Contains(unitActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Slot already exists for %s"), *unitActor->GetName());
		return unitSlotMap[unitActor];
	}
	
	UE_LOG(LogTemp, Warning, TEXT("unitSlotWidget is valid? %s"), unitSlotWidget ? TEXT("YES") : TEXT("NO"));
	
	//없으면 새로 생성
	if (!unitSlotWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("unitSlotWidget is NULL! Cannot create slot."));
        return nullptr;
	}
	
	UK_UnitSlotWiddget* newSlot = CreateWidget<UK_UnitSlotWiddget>(GetWorld(), unitSlotWidget);
	if (newSlot)
	{
		//생성되면 해당 유닛을 슬롯에 연결 후 UI클릭 이벤트 구독
		newSlot->SetLinkedUnit(unitActor);
		newSlot->onSlotClickedDel.AddDynamic(this, &UK_UnitListWidget::OnUnitListClicked);
		
		unitSlotMap.Add(unitActor, newSlot);
		AddUnitSlot(newSlot);
		
		//유닛별 Death Delegate 구독
		AB_UnitBase* unit = Cast<AB_UnitBase>(unitActor);
		if (unit)
		{
			unit->OnUnitDieDelegate.AddDynamic(this, &UK_UnitListWidget::OnUnitDied);
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
	
	UVerticalBoxSlot* childSlot = UnitSlotList->AddChildToVerticalBox(newSlot);
	if (childSlot)
	{
		childSlot->SetPadding(FMargin(0,0,0,10.f));
	}
}
