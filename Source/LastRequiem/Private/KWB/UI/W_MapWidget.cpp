// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/UI/W_MapWidget.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Input/Events.h"
#include "Engine/OverlapResult.h"
#include "GameplayTagsManager.h"
#include "BJM/Unit/B_UnitBase.h"
#include "DrawDebugHelpers.h" // [TEST/DEBUG]

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

void UW_MapWidget::ClearUnitSelection ()
{
	SelectedUnit = nullptr;
	OnMinimapUnitSelected.Broadcast ( nullptr );
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

	// [TEST/DEBUG] 멤버에도 저장 (GetClickedWorldLocation() 정상화)
	ClickedWorldLocation = WorldPos;
	ClickedWidgetLocation = WidgetLocalPos;
	WidgetX = U;
	WidgetY = V;
	WorldX = WorldPos.X;
	WorldY = WorldPos.Y;

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
		// 좌클릭: 클릭 지점 주변에서 가장 가까운 유닛 선택
		AB_UnitBase* NewSelected = FindClosestUnitNear ( WorldPos );
		SelectedUnit = NewSelected;

		// 선택 이벤트 발송(Controller가 받음)
		OnMinimapUnitSelected.Broadcast ( NewSelected );

		// [TEST/DEBUG]
		if (bDebugMinimapSelection && GEngine)
		{
			const FString SelName = NewSelected ? NewSelected->GetName () : TEXT ( "None" );
			GEngine->AddOnScreenDebugMessage (
				-1 , 2.f , FColor::Yellow ,
				FString::Printf ( TEXT ( "[Minimap] Selected: %s" ) , *SelName )
			);
		}
	}
	else if (PressedButton == EKeys::RightMouseButton)
	{
		// 우클릭 : (선택된 유닛 + 목적지) 명령 발송
		OnMinimapMoveCommand.Broadcast ( SelectedUnit.Get () , WorldPos );

		// 레거시 : B_Unit에서 연결 끊으면 삭제 예정
		//OnRightMouseButtonClicked.Broadcast ( WorldPos ); // DEBUG 동안 잠시 주석 처리
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

AB_UnitBase* UW_MapWidget::FindClosestUnitNear ( const FVector& ClickWorld ) const
{
	UWorld* World = GetWorld ();
	if (!World) return nullptr;

	// [TEST/DEBUG]
	const FVector QueryCenter = ClickWorld + FVector ( 0 , 0 , 0 ); // 안되면 SelectionQueryZOffset를 Z값에 넣을 예정

	if (bDebugMinimapSelection)
	{
		DrawDebugSphere ( World , QueryCenter , UnitSelectionRadius , 24 , FColor::Cyan , false , DebugDrawDuration );
		DrawDebugString ( World , QueryCenter + FVector ( 0 , 0 , 50 ) , TEXT ( "Minimap Pick Center" ) , nullptr , FColor::Cyan , DebugDrawDuration );
	}
	//

	TArray<FOverlapResult> Overlaps;
	const FCollisionShape Shape = FCollisionShape::MakeSphere ( UnitSelectionRadius );

	const bool bHit = World->OverlapMultiByChannel (
		Overlaps ,
		ClickWorld ,
		FQuat::Identity ,
		ECC_Pawn ,
		Shape
	);

	// [TEST/DEBUG]
	if (!bHit)
	{
		if (bDebugLogOverlaps)
		{
			UE_LOG ( LogTemp , Warning , TEXT ( "[MinimapPick] No overlap at %s" ) , *QueryCenter.ToString () );
		}
		return nullptr;
	}
	//

	// 디버그 이후 살릴 것
	//if (!bHit) return nullptr;

	AB_UnitBase* BestUnit = nullptr;
	float BestDistSq = TNumericLimits<float>::Max ();

	// [TEST/DEBUG]
	if (bDebugLogOverlaps)
	{
		UE_LOG ( LogTemp , Log , TEXT ( "[MinimapPick] Overlaps=%d" ) , Overlaps.Num () );
	}
	//

	for (const FOverlapResult& R : Overlaps)
	{
		AB_UnitBase* Unit = Cast<AB_UnitBase> ( R.GetActor () );
		if (!Unit) continue;
		if (!Unit->IsAlive ()) continue;

		// 태그 필터: SelectableUnitTag(Unit) 없는 액터는 선택 제외
		if (SelectableUnitTag.IsValid ())
		{
			FGameplayTagContainer OwnedTags;
			Unit->GetOwnedGameplayTags ( OwnedTags ); // AB_UnitBase가 GameplayTags를 반환
			if (!OwnedTags.HasTag ( SelectableUnitTag ))
			{
				continue;
			}
		}

		const float DistSq = FVector::DistSquared2D ( Unit->GetActorLocation () , ClickWorld );

		// [TEST/DEBUG]
		if (bDebugMinimapSelection)
		{
			DrawDebugSphere ( World , Unit->GetActorLocation () , 60.f , 12 , FColor::White , false , DebugDrawDuration );
			DrawDebugLine ( World , QueryCenter , Unit->GetActorLocation () , FColor::White , false , DebugDrawDuration , 0 , 1.f );
			DrawDebugString ( World , Unit->GetActorLocation () + FVector ( 0 , 0 , 120 ) ,
				Unit->GetName () , nullptr , FColor::White , DebugDrawDuration );
		}
		//

		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestUnit = Unit;
		}
	}

	// [TEST/DEBUG]
	if (bDebugMinimapSelection && BestUnit)
	{
		DrawDebugSphere ( World , BestUnit->GetActorLocation () , 120.f , 16 , FColor::Green , false , DebugDrawDuration );
		DrawDebugString ( World , BestUnit->GetActorLocation () + FVector ( 0 , 0 , 180 ) ,
			TEXT ( "SELECTED" ) , nullptr , FColor::Orange , DebugDrawDuration );
	}
	//

	return BestUnit;
}
