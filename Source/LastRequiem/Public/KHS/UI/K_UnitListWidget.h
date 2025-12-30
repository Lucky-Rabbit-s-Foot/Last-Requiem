// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K_UnitSlotWiddget.h"
#include "KHS/UI/K_BaseUIWidget.h"
#include "K_UnitListWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitListSelected, AActor*, selectedUnit);

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

	UFUNCTION()
	void OnUnitDied(AActor* deadUnit);
	
	UFUNCTION()
	void OnUnitListClicked(AActor* selectedUnit);
	
	UK_UnitSlotWiddget* GetOrCreateUnitSlot(AActor* unitActor);

public:
	void AddUnitSlot(UK_UnitSlotWiddget* newSlot);

private:
	//드론용 델리게이트 핸들
	FDelegateHandle DetectionDelegateHandle;
	FDelegateHandle LostDetectionDelegateHandle;
	
	//유닛-슬롯 맵핑
	UPROPERTY()
	TMap<AActor*, UK_UnitSlotWiddget*> unitSlotMap;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* UnitSlotList;
	
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI")
	TSubclassOf<class UK_UnitSlotWiddget> unitSlotWidget;
	
	// (20251226) W : 에디터에서 슬롯 간격 조정하기 위해 추가
	UPROPERTY ( EditDefaultsOnly , Category = "LR|UI" )
	float UnitSlotSpacing = 10.f;

public:
	UPROPERTY(BlueprintAssignable, Category = "LR|Event")
	FOnUnitListSelected onUnitListSelectedDel;

// 종민 수정
protected:
	UFUNCTION ()
	void OnUnitSpeakChanged ( AB_UnitBase* Unit , bool bIsSpeaking , FString StateText );
	
};
