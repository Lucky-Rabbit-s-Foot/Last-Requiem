// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/W_MapWidget.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Input/Events.h"
#include "Engine/OverlapResult.h"
#include "GameplayTagsManager.h"
#include "BJM/Unit/B_UnitBase.h"
#include "DrawDebugHelpers.h" // [TEST/DEBUG]
#include "Kismet/GameplayStatics.h"
#include "KHS/Util/K_LoggingSystem.h"

void UW_MapWidget::NativeConstruct ()
{
	Super::NativeConstruct ();

	// SelectableUnitTag가 비어있으면 기본 "Unit" 사용
	if (!SelectableUnitTag.IsValid ())
	{
		SelectableUnitTag = FGameplayTag::RequestGameplayTag ( FName ( "Unit" ) );
	}

	// TEST: TEMP Func => 형님 코드랑 충돌 안되는 지 확인 필요
	if (GetOwningPlayer ())
	{
		GetOwningPlayer ()->bShowMouseCursor = true;
	}
}

AActor* UW_MapWidget::GetUnitAtWorldPosition ( const FVector& WorldPos )
{
	// 거리계산 방식: 모든 유닛을 순회하며 가장 가까운 유닛 찾기
	TArray<AActor*> FoundUnits;
	UGameplayStatics::GetAllActorsOfClass ( GetWorld () , AB_UnitBase::StaticClass () , FoundUnits );

	AActor* ClosestUnit = nullptr;
	float MinDistance = UnitSelectionRadius; // 설정된 반경 이내만

	for (AActor* UnitActor : FoundUnits)
	{
		AB_UnitBase* Unit = Cast<AB_UnitBase> ( UnitActor );
		if (!Unit || !Unit->IsAlive ()) continue;

		// 2D 거리 계산 (Z축 무시)
		float Distance = FVector::Dist2D ( Unit->GetActorLocation () , WorldPos );

		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestUnit = UnitActor;
		}
	}

	return ClosestUnit;
}

void UW_MapWidget::ClearUnitSelection ()
{
	SelectedUnit = nullptr;
	OnMapUnitSelected.Broadcast ( nullptr );
}

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

	// [FIX] 멤버에 저장 (이걸 안 해서 선택이 안 됨)
	ClickedWorldLocation = WorldPos;
	ClickedWidgetLocation = WidgetLocalPos; // 필요하면

	// 8. 디버그 출력
	
	UE_LOG ( LogTemp , Log , TEXT ( "Minimap Click: U=%.3f V=%.3f -> World=%s" ) ,
		U , V , *WorldPos.ToString () );
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage (
			-1 , 2.f , FColor::Green ,
			FString::Printf ( TEXT ( "WorldPos: %s" ) , *WorldPos.ToString () )
		);
	}*/
	KHS_SCREEN_INFO ( TEXT ( "WorldPos: %s" ) , *WorldPos.ToString () );


	// 좌클릭: 유닛 선택
	if (PressedButton == EKeys::LeftMouseButton)
	{
		OnLeftMouseButtonClicked.Broadcast ();

		AActor* ClickedUnit = GetUnitAtWorldPosition ( ClickedWorldLocation );
		if (ClickedUnit)
		{
			SelectedUnit = ClickedUnit;
			OnMapUnitSelected.Broadcast ( ClickedUnit );
			KHS_INFO ( TEXT ( "Unit Selected: %s" ) , *ClickedUnit->GetName () );
		}
		else
		{
			// 빈 곳 클릭 시 선택 해제
			SelectedUnit = nullptr;
			KHS_INFO ( TEXT ( "No unit at clicked position" ) );
		}

		return FReply::Handled ();
	}

	// 우클릭: 이동 명령
	if (PressedButton == EKeys::RightMouseButton)
	{
		OnRightMouseButtonClicked.Broadcast ( ClickedWorldLocation );

		if (SelectedUnit.IsValid ())
		{
			OnMapMoveCommand.Broadcast ( SelectedUnit.Get () , ClickedWorldLocation );
			KHS_INFO ( TEXT ( "Move command: %s to %s" ) ,
				*SelectedUnit->GetName () , *ClickedWorldLocation.ToString () );
		}

		return FReply::Handled ();
	}

	return Super::NativeOnMouseButtonDown ( InGeometry , InMouseEvent );
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

// 레거시 : Overlap 사용해서 유닛 선택하기 -> 거리 계산으로 셀렉 동작하면 지울 예정
//AActor* UW_MapWidget::FindClosestUnitNear ( const FVector& ClickWorld ) const
//{
//	UWorld* World = GetWorld ();
//	if (!World) return nullptr;
//
//	TArray<FOverlapResult> Overlaps;
//	const FCollisionShape Shape = FCollisionShape::MakeSphere ( UnitSelectionRadius );
//
//	const bool bHit = World->OverlapMultiByChannel (
//		Overlaps ,
//		ClickWorld ,
//		FQuat::Identity ,
//		ECC_Pawn ,
//		Shape
//	);
//
//	if (!bHit) return nullptr;
//
//	AB_UnitBase* BestUnit = nullptr;
//	float BestDistSq = TNumericLimits<float>::Max ();
//
//	for (const FOverlapResult& R : Overlaps)
//	{
//		AB_UnitBase* Unit = Cast<AB_UnitBase> ( R.GetActor () );
//		if (!Unit) continue;
//		if (!Unit->IsAlive ()) continue;
//
//		// 태그 필터: SelectableUnitTag(Unit) 없는 액터는 선택 제외
//		if (SelectableUnitTag.IsValid ())
//		{
//			FGameplayTagContainer OwnedTags;
//			Unit->GetOwnedGameplayTags ( OwnedTags ); // AB_UnitBase가 GameplayTags를 반환
//			if (!OwnedTags.HasTag ( SelectableUnitTag ))
//			{
//				continue;
//			}
//		}
//
//		const float DistSq = FVector::DistSquared2D ( Unit->GetActorLocation () , ClickWorld );
//
//		if (DistSq < BestDistSq)
//		{
//			BestDistSq = DistSq;
//			BestUnit = Unit;
//		}
//	}
//
//	return BestUnit;
//}
