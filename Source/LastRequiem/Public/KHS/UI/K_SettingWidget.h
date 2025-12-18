// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KHS/UI/K_BaseUIWidget.h"
#include "K_SettingWidget.generated.h"

class UButton;

//컨트롤러에 요청할 버튼 바인딩 함수 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRestartRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuitGameRequested);


UCLASS()
class LASTREQUIEM_API UK_SettingWidget : public UK_BaseUIWidget
{
	GENERATED_BODY()
	
	UK_SettingWidget();
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
private:
	UFUNCTION()
	void OnClickedTutorialButton();
	
	UFUNCTION()
	void OnClickedBackButton();
	
	UFUNCTION()
	void OnClickedRestartButton();
	
	UFUNCTION()
	void OnClickedQuitButton();	
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UImage* LogoImg;
	
	UPROPERTY(meta=(BindWidget))
	UButton* TutorialButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* BackButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* RestartButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* QuitButton;
	
public:
	UPROPERTY(BlueprintAssignable, Category="LR|Events")
	FOnTutorialRequested onTutorialRequestedDel;
	
	UPROPERTY(BlueprintAssignable, Category="LR|Events")
	FOnRestartRequested onRestartRequestedDel;
	
	UPROPERTY(BlueprintAssignable, Category="LR|Events")
	FOnQuitGameRequested onQuitGameRequestedDel;
	
	
};
