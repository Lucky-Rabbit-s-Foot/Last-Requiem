// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/UI/K_BaseUIWidget.h"
#include "K_TutorialWidget.generated.h"

class UButton;


UCLASS()
class LASTREQUIEM_API UK_TutorialWidget : public UK_BaseUIWidget
{
	GENERATED_BODY()
	
	UK_TutorialWidget();
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UFUNCTION()
	void OnClickedQuitButton();
	
	UFUNCTION()
	void OnClickedNextImageButton();
	
	UFUNCTION()
	void OnClickedPreviousImageButton();
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* ExitButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* NextButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* PrevButton;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* TutorialImg;
	
	//TODO : 나중에 여기다가 튜토리얼 이미지 텍스쳐 Map or Set 만들어서
	//NEXT/PREV 버튼으로 이미지 인덱스 넘겨서 다음 이미지 표출하는 작업하면됨.;
	
};
