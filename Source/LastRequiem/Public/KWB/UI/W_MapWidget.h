// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_MapWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnLeftMouseButtonClicked );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnRightMouseButtonClicked, FVector, OutClickedLocation);
/**
 * 
 */
UCLASS()
class LASTREQUIEM_API UW_MapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct () override;

	inline FVector GetClickedWorldLocation () { return ClickedWorldLocation; }

protected:
	virtual FReply NativeOnMouseButtonDown ( const FGeometry& InGeometry , const FPointerEvent& InMouseEvent ) override;
	
	FVector WidgetPosToWorldPos ( FVector InWidgetLocation );

	FVector MapUVToWorld ( float U , float V ) const;

public:
	// 맵 중심 (Location)
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "UI|Data|Map" )
	FVector MapCenterWorld = FVector( 10000.0f , 0.0f, -0.5f ); // TEST : KWB 월드 기준으로 맞춘 것

	// 맵 월드 가로 길이
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "UI|Data|Map" )
	float MapWorldWidth = 8000.0f; // TEST : KWB 월드 기준으로 맞춘 것

	// 맵 월드 세로 길이
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "UI|Data|Map" )
	float MapWorldHeight = 8000.0f; // TEST : KWB 월드 기준으로 맞춘 것

	// 바닥 높이 (Z 고정 값)
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "UI|Data|Map" )
	float GroundZ = -0.5f; // TEST : KWB 월드 기준으로 맞춘 것

	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Minimap" )
	float MapYaw = 0.f; // 맵이 월드에서 도는 각도 (필요하면)

	// 델리게이트
	FOnLeftMouseButtonClicked OnLeftMouseButtonClicked;
	FOnRightMouseButtonClicked OnRightMouseButtonClicked;

private:
	// X, Y의 좌표가 각각 필요할 경우가 있을 것 같아 나눠서 저장함 (ex. 스킬 : 라인 따라 발사되는 레이저, 라인으로 전개되는 바리케이드 etc..)
	// 위젯 상의 X좌표 (0 ~ 1)
	float WidgetX = 0.0f;

	// 위젯 상의 Y좌표 (0 ~ 1) 
	float WidgetY = 0.0f;

	// 월드 상의 X좌표 (기준값 맵 설정에 따라 상이할 수 있음)
	float WorldX = 0.0f;

	// 월드 상의 Y좌표 (기준값 맵 설정에 따라 상이할 수 있음)
	float WorldY = 0.0f;

	// 마우스가 클릭하고 얻은 월드 상의 위치 좌표 값(Absolute) 저장
	FVector2D ClickedWidgetLocation;

	// 마우스가 클릭하고 얻은 월드 상의 위치 좌표 값 저장
	FVector ClickedWorldLocation;

};
