// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K_UnitSlotWiddget.h"
#include "KHS/UI/K_BaseUIWidget.h"
#include "K_UnitListWidget.generated.h"

/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UK_UnitListWidget : public UK_BaseUIWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void OnUnitDetected(AActor* detectedUnit);
	void OnUnitLostDetection(AActor* lostUnit);
	
	UK_UnitSlotWiddget* GetOrCreateUnitSlot(AActor* unitActor);
	
public:
	void AddUnitSlot(UK_UnitSlotWiddget* newSlot);

private:
	FDelegateHandle DetectionDelegateHandle;
	FDelegateHandle LostDetectionDelegateHandle;
	TMap<AActor*, FDelegateHandle> DeathDeleageMap;
	
	UPROPERTY()
	TMap<AActor*, UK_UnitSlotWiddget*> unitSlotMap;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* UnitSlotList;
	
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class UK_UnitSlotWiddget> unitSlotWidget;
	
};
