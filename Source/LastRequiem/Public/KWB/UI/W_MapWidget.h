// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "W_MapWidget.generated.h"

class AActor;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnLeftMouseButtonClicked );
// 종민님 쪽에서 삭제 해야 삭제할 수 있음
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnRightMouseButtonClicked, FVector, OutClickedLocation);

// 유닛 선택 / 선택된 유닛에게 이동명령
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam ( FOnMapUnitSelected , AActor* , SelectedActor );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams ( FOnMapMoveCommand , AActor* , SelectedActor , FVector , Destination );
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

	// 거리계산으로 유닛 찾기
	AActor* GetUnitAtWorldPosition ( const FVector& WorldPos );

	UFUNCTION ( BlueprintCallable , Category = "Minimap" )
	void SetSelectedUnit ( AActor* Unit ) { SelectedUnit = Unit; }

	UFUNCTION ( BlueprintCallable , Category = "Minimap" )
	AActor* GetSelectedUnit () const { return SelectedUnit.Get (); }

	UFUNCTION ( BlueprintCallable , Category = "Minimap" )
	void ClearUnitSelection ();

protected:
	virtual FReply NativeOnMouseButtonDown ( const FGeometry& InGeometry , const FPointerEvent& InMouseEvent ) override;

	FVector MapUVToWorld ( float U , float V ) const;

	// 레거시 : Overlap 사용해서 유닛 선택하기 -> 거리 계산으로 셀렉 동작하면 지울 예정
	// 클릭 근처에서 유닛 찾기
	// AActor* FindClosestUnitNear ( const FVector& ClickWorld ) const;

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

	// 유닛 선택 반경
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Minimap|Selection" )
	float UnitSelectionRadius = 100.f; // 인디케이터 크기에 따라 변경되어야 함

	// 선택 가능한 유닛을 태그로 제한 (Default: Unit => cpp에서 설정)
	UPROPERTY ( EditAnywhere , BlueprintReadWrite , Category = "Minimap|Selection" )
	FGameplayTag SelectableUnitTag;

	// 델리게이트
	UPROPERTY ( BlueprintAssignable , Category = "Map|Event" )
	FOnMapUnitSelected OnMapUnitSelected;

	UPROPERTY ( BlueprintAssignable , Category = "Map|Event" )
	FOnMapMoveCommand OnMapMoveCommand;

	//FOnLeftMouseButtonClicked OnLeftMouseButtonClicked;
	//// 레거시 : 종민님 코드 수정 후 제거 || 수정된 코드와 호환성 체크 후 제거 여부 결정
	FOnRightMouseButtonClicked OnRightMouseButtonClicked;

private:
	// 선택된 유닛 저장용
	TWeakObjectPtr<AActor> SelectedUnit;

	//// 마우스가 클릭하고 얻은 월드 상의 위치 좌표 값(Absolute) 저장
	//FVector2D ClickedWidgetLocation;

	// 마우스가 클릭하고 얻은 월드 상의 위치 좌표 값 저장
	FVector ClickedWorldLocation;

};
