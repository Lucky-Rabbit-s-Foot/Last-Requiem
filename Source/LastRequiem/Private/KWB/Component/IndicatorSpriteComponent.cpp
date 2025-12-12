// Fill out your copyright notice in the Description page of Project Settings.


#include "KWB/Component/IndicatorSpriteComponent.h"

UIndicatorSpriteComponent::UIndicatorSpriteComponent ()
{
	// 타이머로 해결할 수 있으면 끄기
	PrimaryComponentTick.bCanEverTick = true;

	if (SpirteNormal)
	{
		SourceSprite = SpirteNormal;
	}
	else
	{
		UE_LOG (LogTemp, Warning, TEXT("기본 SpriteNormal 데이터가 존재하지 않습니다!"));
	}

	GlowSpeed = 5.0f;
	GlowMin = 0.5f;
	GlowMax = 2.0f;

	bGlowActive = false;
	GlowTime = 0.0f;

	// 충돌, 그림자, 정렬 등 기본값
	SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	SetCastShadow ( false );
	SetReceivesDecals ( false );

	// 미니맵에서만 보이게
	bVisibleInSceneCaptureOnly = true;			 // SceneCapture에서만 표시 
	bHiddenInGame = false;						 // 게임에서 숨기지 않는다 (SceneCapture 포함) : TEST - 테스트 후 필요시 세팅
	SetVisibility ( true , true );

	// 투명 객체 정렬 우선순위 올리기 : 테스트 후 필요할 시 세팅
	// TranslucencySortPriority = 10;
}

void UIndicatorSpriteComponent::BeginPlay ()
{

}

void UIndicatorSpriteComponent::TickComponent ( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction )
{

}

void UIndicatorSpriteComponent::UpdateSpriteForState ()
{
}

void UIndicatorSpriteComponent::UpdateGlow ( float DeltaTime )
{
}
