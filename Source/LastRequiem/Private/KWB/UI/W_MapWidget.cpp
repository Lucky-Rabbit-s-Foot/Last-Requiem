// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/W_MapWidget.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Input/Events.h"
#include "Engine/OverlapResult.h"
#include "BJM/Unit/B_UnitBase.h"

void UW_MapWidget::NativeConstruct ()
{
	Super::NativeConstruct ();

	// TEST: TEMP Func
	if (GetOwningPlayer ())
	{
		GetOwningPlayer ()->bShowMouseCursor = true;
	}
}

// FIX : 함수 이름 수정 필요 여기까지 하던 중
//void UW_MapWidget::ClearSelection ()
//{
//	SelectedActor = nullptr;
//	OnMinimapUnitSelected.Broadcast ( nullptr );
//}

FReply UW_MapWidget::NativeOnMouseButtonDown ( const FGeometry& InGeometry , const FPointerEvent& InMouseEvent )
{
	FKey PressedButton = InMouseEvent.GetEffectingButton ();
	
	if (PressedButton != EKeys::LeftMouseButton &&
		PressedButton != EKeys::RightMouseButton)
	{
		return Super::NativeOnMouseButtonDown ( InGeometry , InMouseEvent );
	}

	// 1. 전체 스크린 좌표
	const FVector2D ScreenPos = InMouseEvent.GetScreenSpacePosition ();

	// 2. MapWidget의 로컬 좌표로 변환
	const FVector2D WidgetLocalPos = InGeometry.AbsoluteToLocal ( ScreenPos );

	// 3. MapWidget의 실제 크기 (DPI, 부모 스케일 반영된 값)
	const FVector2D WidgetLocalSize = InGeometry.GetLocalSize ();

	// 방어 코드: 크기가 0이면 계산 불가
	if (WidgetLocalSize.X <= 0.f || WidgetLocalSize.Y <= 0.f)
	{
		return FReply::Unhandled ();
	}

	// 4. 0~1로 정규화 (좌상단 기준)
	float U_top = WidgetLocalPos.X / WidgetLocalSize.X;
	float V_top = WidgetLocalPos.Y / WidgetLocalSize.Y;

	// 5. 범위 밖 클릭은 맵 밖이므로 0~1로 클램프
	U_top = FMath::Clamp ( U_top , 0.f , 1.f );
	V_top = FMath::Clamp ( V_top , 0.f , 1.f );

	// 6. 기준점(0, 0)을 좌하단으로 잡고, 월드 방향 방향과 위젯의 방향 일치시키기
	const float U = U_top;
	const float V = 1.f - V_top;

	// 7. (U,V) → 월드 좌표 변환
	const FVector WorldPos = MapUVToWorld ( U , V );

	// 8. 디버그 출력
	UE_LOG ( LogTemp , Log , TEXT ( "Minimap Click: U=%.3f V=%.3f -> World=%s" ) ,
		U , V , *WorldPos.ToString () );
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage (
			-1 , 2.f , FColor::Green ,
			FString::Printf ( TEXT ( "WorldPos: %s" ) , *WorldPos.ToString () )
		);
	}

	if (PressedButton == EKeys::LeftMouseButton)
	{
		// 좌클릭은 액터를 선택하거나 액션 버튼을 조작하는 기능을 담당
		// 1. 선택한 부분이 액터일 때

		// 2. 선택한 부분이 

		OnLeftMouseButtonClicked.Broadcast ();
	}
	else if (PressedButton == EKeys::RightMouseButton)
	{
		// 우클릭은 위치 값을 유닛에게 전달

		// 가장 마지막에 할 것
		OnRightMouseButtonClicked.Broadcast ( WorldPos );
		UE_LOG ( LogTemp , Warning , TEXT ( "World Loc Val 반환 델리게이트 발송 완료" ) );
	}

	return FReply::Handled ();
}

FVector UW_MapWidget::MapUVToWorld ( float U , float V ) const
{
	// 1. 맵 로컬 기준 오프셋 계산
	const float OffsetX = (V - 0.5f) * MapWorldHeight; // 위/아래 방향이 월드 X
	const float OffsetY = (U - 0.5f) * MapWorldWidth;  // 좌/우 방향이 월드 Y

	FVector Offset ( OffsetX , OffsetY , 0.f );

	// 2. 맵이 월드에서 회전해 있다면 회전 보정 (필요 없으면 MapYaw=0)
	if (!FMath::IsNearlyZero ( MapYaw ))
	{
		const FRotator Rot ( 0.f , MapYaw , 0.f );
		Offset = Rot.RotateVector ( Offset );
	}

	// 3. 중심 위치 + 오프셋 = 최종 XY, Z는 바닥 높이로 고정
	FVector WorldPos = MapCenterWorld + Offset;
	WorldPos.Z = GroundZ;

	return WorldPos;
}

AActor* UW_MapWidget::FindClosestUnitNear ( const FVector& ClickWorld ) const
{
	return nullptr;
}
