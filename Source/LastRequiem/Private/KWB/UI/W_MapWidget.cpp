// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/W_MapWidget.h"

void UW_MapWidget::NativeConstruct ()
{
	Super::NativeConstruct ();

	// TEST: TEMP Func
	if (GetOwningPlayer ())
	{
		GetOwningPlayer ()->bShowMouseCursor = true;
	}
}

FReply UW_MapWidget::NativeOnMouseButtonDown ( const FGeometry& InGeometry , const FPointerEvent& InMouseEvent )
{
	FKey PressedButton = InMouseEvent.GetEffectingButton ();
	
	if (PressedButton != EKeys::LeftMouseButton &&
		PressedButton != EKeys::RightMouseButton)
	{
		return Super::NativeOnMouseButtonDown ( InGeometry , InMouseEvent );
	}

	/* TEST : 실사용할 코드 아래 코드 테스트 위해 잠시 가림 */
	//FVector2D ClickedLocation = InMouseEvent.GetScreenSpacePosition ();
	//// 해당 위젯을 기준으로 실제 크기에 맞춰서 좌표값 변환
	//ClickedWidgetLocation = InGeometry.AbsoluteToLocal ( ClickedWidgetLocation );

	// 2. 화면(스크린) 좌표
	const FVector2D ScreenPos = InMouseEvent.GetScreenSpacePosition ();

	// 3. 이 위젯(또는 맵 이미지)의 로컬 좌표로 변환
	//    - 여기서는 InGeometry 자체가 맵 영역이라고 가정
	const FVector2D LocalPos = InGeometry.AbsoluteToLocal ( ScreenPos );

	// 4. 이 위젯의 실제 크기 (DPI, 부모 스케일 다 반영된 값)
	const FVector2D LocalSize = InGeometry.GetLocalSize ();

	// 방어 코드: 크기가 0이면 계산 불가
	if (LocalSize.X <= 0.f || LocalSize.Y <= 0.f)
	{
		return FReply::Unhandled ();
	}

	// 5. 0~1로 정규화 (좌상단 기준)
	float U_top = LocalPos.X / LocalSize.X;
	float V_top = LocalPos.Y / LocalSize.Y;

	// 6. 범위 밖 클릭은 맵 밖이므로 0~1로 클램프
	U_top = FMath::Clamp ( U_top , 0.f , 1.f );
	V_top = FMath::Clamp ( V_top , 0.f , 1.f );

	// 7. 좌하단을 (0,0)으로 쓰고 싶으니 Y 뒤집기 => 불필요한 연산이라 우선 뒤집는 기능 제거
	const float U = U_top;
	const float V = 1.f - V_top;

	// 8. (U,V) → 월드 좌표 변환
	const FVector WorldPos = MapUVToWorld ( U , V );

	// 9. 디버그 출력
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
		OnLeftMouseButtonClicked.Broadcast ();	// 굳이 필요하지 않을 수도 있음
	
	}
	else if (PressedButton == EKeys::RightMouseButton)
	{
		// 우클릭은 위치 값을 유닛에게 전달

		// 가장 마지막에 할 것
		OnRightMouseButtonClicked.Broadcast ( WorldPos );
	}

	return FReply::Handled ();
}

FVector UW_MapWidget::WidgetPosToWorldPos ( FVector InWidgetLocation )
{
	

	return ClickedWorldLocation;
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