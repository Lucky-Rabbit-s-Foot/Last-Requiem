// IndicatorSpriteComponent.cpp

#include "KWB/Component/IndicatorSpriteComponent.h"
#include "PaperSprite.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "TimerManager.h"

// FPropertyChangedEvent
#if WITH_EDITOR
#include "UObject/UnrealType.h"
#endif

UIndicatorSpriteComponent::UIndicatorSpriteComponent ()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Default Transform
	DefaultRelativeTransform = FTransform (
		FRotator ( 0.f , 0.f , -90.f ) ,	// 기준 순서 : Pitch, Yaw, Roll | 엔진에서는 Roll, Pitch, Yaw
		FVector ( 0.f , 0.f , 3000.f ) ,	// TEMP : 레벨 구성 완료 후 결정 / 3000.0f로 테스트
		FVector ( 0.35f , 0.35f , 0.35f )   // TEMP : 알파 게임 플레이 후 결정 !!사이즈 변경 시 W_MapWidget에서 UnitSelectionRadius 값 변경할 것!!
	);

	// 상태/Glow 기본값
	CurrentState = EIndicatorSpriteState::Normal;
	bSpritesInitialized = false;

	DynamicMID = nullptr;
	bGlowActive = false;
	GlowTime = 0.0f;

	// 스프라이트 설정
	SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	SetCastShadow ( false );
	SetReceivesDecals ( false );

	// TEST : 확인 필요
	bVisibleInSceneCaptureOnly = true;	// SceneCapture에서만 표시
	bHiddenInGame = false;
	SetVisibility ( true , false );	// propagate 끔 => true -> false : P_Obstacle 디버그
}

void UIndicatorSpriteComponent::BeginPlay ()
{
	Super::BeginPlay ();

	// 런타임 보정 1회
	ApplyDefaultTransformIfNeeded ( false );

	EnsureDynamicMaterial ();

	// 초기 상태로 스프라이트 설정
	CurrentState = InitialState;
	InitializeSpritesIfNeeded ();
	UpdateSpriteForState ();
}

void UIndicatorSpriteComponent::OnRegister ()
{
	Super::OnRegister ();

	if (IsTemplate ())
	{
		return;
	}

	// 재진입 가드 (Stack Overflow 방지)
	if (bInOnRegister)
	{
		return;
	}
	bInOnRegister = true;

	// OnRegister 스택 안에서 Transform 변경 금지 -> 다음 틱에 1회 적용
	if (bApplyDefaultTransform && !bDefaultTransformApplied)
	{
		if (UWorld* World = GetWorld ())
		{
			World->GetTimerManager ().SetTimerForNextTick (
				FTimerDelegate::CreateUObject ( this , &UIndicatorSpriteComponent::ApplyDefaultTransformDeferred )
			);
		}
		else
		{
			ApplyDefaultTransformIfNeeded ( false );
		}
	}

	ApplyVisibilityRules ();
	EnsureDynamicMaterial ();

	// 에디터 프리뷰에서는 InitialState가 보이도록만 보정
	if (UWorld* World = GetWorld ())
	{
		if (!World->IsGameWorld ())
		{
			CurrentState = InitialState;
		}
	}

	InitializeSpritesIfNeeded ();
	UpdateSpriteForState ();

	bInOnRegister = false;
}

void UIndicatorSpriteComponent::OnComponentCreated ()
{
	Super::OnComponentCreated ();

	if (IsTemplate ())
	{
		return;
	}

	// 여기서는 “적용 시도” 정도만(실제 강한 보정은 PostLoad/OnRegister 디퍼로)
	ApplyDefaultTransformIfNeeded ( false );
}

void UIndicatorSpriteComponent::PostLoad ()
{
	Super::PostLoad ();

	if (IsTemplate ())
	{
		return;
	}

	// 로드 후에는 BP/자산 기본값이 모두 반영된 상태이므로 강제 1회 재적용이 안전
	ApplyDefaultTransformIfNeeded ( true );
}

#if WITH_EDITOR
void UIndicatorSpriteComponent::PostEditChangeProperty ( FPropertyChangedEvent& PropertyChangedEvent ) // [ADDED]
{
	Super::PostEditChangeProperty ( PropertyChangedEvent );

	if (IsTemplate ())
	{
		return;
	}

	const FName PropName = PropertyChangedEvent.GetPropertyName ();

	// Transform/옵션 변경 시 즉시 반영
	if (PropName == GET_MEMBER_NAME_CHECKED ( UIndicatorSpriteComponent , DefaultRelativeTransform ) ||
		PropName == GET_MEMBER_NAME_CHECKED ( UIndicatorSpriteComponent , bApplyDefaultTransform ))
	{
		ApplyDefaultTransformIfNeeded ( true );
	}

	// 가시성 옵션 변경 즉시 반영
	if (PropName == GET_MEMBER_NAME_CHECKED ( UIndicatorSpriteComponent , bVisibleOnlyInSceneCapture ))
	{
		ApplyVisibilityRules ();
	}
}
#endif

void UIndicatorSpriteComponent::TickComponent ( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent ( DeltaTime , TickType , ThisTickFunction );

	if (bGlowActive)
	{
		UpdateGlow ( DeltaTime );
	}
}

void UIndicatorSpriteComponent::SetIndicatorState ( EIndicatorSpriteState NewState )
{
	// 방어
	if (IsTemplate ())
	{
		return;
	}

	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;

	InitializeSpritesIfNeeded ();
	UpdateSpriteForState ();
}

void UIndicatorSpriteComponent::StartGlow ( float DurationSeconds )
{
	if (IsTemplate ())
	{
		return;
	}

	bGlowActive = true;
	GlowTime = 0.0f;

	EnsureDynamicMaterial ();

	// Tick 켜주기
	PrimaryComponentTick.SetTickFunctionEnable ( true );

	if (DurationSeconds > 0.0f)
	{
		if (UWorld* World = GetWorld ())
		{
			World->GetTimerManager ().ClearTimer ( GlowTimer );
			World->GetTimerManager ().SetTimer (
				GlowTimer ,
				this ,
				&UIndicatorSpriteComponent::HandleGlowFinished ,
				DurationSeconds ,
				false
			);
		}
	}
}

void UIndicatorSpriteComponent::StopGlow ()
{
	if (IsTemplate ())
	{
		return;
	}

	bGlowActive = false;

	// Tick 끄기 (다른 용도로 Tick 쓸 거면 이 부분은 상황에 따라 조정)
	PrimaryComponentTick.SetTickFunctionEnable ( false );

	if (DynamicMID)
	{
		// Glow 기본값으로 되돌리기 (머티리얼에서 기본값 1.0으로 맞췄다고 가정)
		DynamicMID->SetScalarParameterValue ( TEXT ( "Glow" ) , 1.0f );
	}

	if (UWorld* World = GetWorld ())
	{
		World->GetTimerManager ().ClearTimer ( GlowTimer );
	}
}

void UIndicatorSpriteComponent::SetSpriteOnOff ( bool bOn , bool bStopGlowWhenOff )
{
	if (IsTemplate ())
	{
		return;
	}

	if (bSpriteOn == bOn)
	{
		return;
	}

	bSpriteOn = bOn;

	// SceneCapture 포함 렌더링 자체 On/Off
	SetVisibility ( bSpriteOn , false );	// propagate 끔 => true -> false : P_Obstacle 디버그

	// Glow 같이 끄기
	if (!bSpriteOn && bStopGlowWhenOff)
	{
		StopGlow ();
	}
}

void UIndicatorSpriteComponent::ToggleSpriteOnOff ()
{
	SetSpriteOnOff ( !bSpriteOn );
}

void UIndicatorSpriteComponent::HandleGlowFinished ()
{
	StopGlow ();
}

void UIndicatorSpriteComponent::ApplyVisibilityRules ()
{
	if (bVisibleOnlyInSceneCapture)
	{
		bVisibleInSceneCaptureOnly = true;
		bHiddenInGame = false;
	}
	else
	{
		bVisibleInSceneCaptureOnly = false;
	}

	SetVisibility ( bSpriteOn , false );
}

void UIndicatorSpriteComponent::EnsureDynamicMaterial ()
{
	if (DynamicMID)
	{
		return;
	}

	if (GetNumMaterials () > 0)
	{
		if (UMaterialInterface* BaseMat = GetMaterial ( 0 ))
		{
			DynamicMID = CreateDynamicMaterialInstance ( 0 , BaseMat );
		}
	}
}

void UIndicatorSpriteComponent::ApplyDefaultTransformIfNeeded ( bool bForce )
{
	if (!bApplyDefaultTransform)
	{
		return;
	}

	if (bDefaultTransformApplied && !bForce)
	{
		return;
	}

	if (IsTemplate ())
	{
		return;
	}

	if (bForce)
	{
		bDefaultTransformApplied = false;
	}

	// 먼저 플래그를 올려서 재등록 루프를 끊음
	bDefaultTransformApplied = true;

	SetRelativeTransform ( DefaultRelativeTransform );
}

void UIndicatorSpriteComponent::ApplyDefaultTransformDeferred ()
{
	ApplyDefaultTransformIfNeeded ( false );
}

void UIndicatorSpriteComponent::InitializeSpritesIfNeeded ()
{
	if (bSpritesInitialized)
	{
		return;
	}
	bSpritesInitialized = true;

	// 최소 하나는 설정되어 있어야 함
	UPaperSprite* Fallback = SpirteNormal;
	if (!Fallback) Fallback = SpirteCombat;
	if (!Fallback) Fallback = SpirteSelected;
	if (!Fallback) Fallback = SpirteDead;

	if (!Fallback)
	{
		UE_LOG ( LogTemp , Warning ,
			TEXT ( "IndicatorSpriteComponent: %s에 어떤 Sprite도 설정되어 있지 않습니다." ) ,
			*GetOwner ()->GetName () );
		return;
	}

	// Drone처럼 Normal만 쓰는 경우를 위해 나머지 비어 있으면 Fallback으로 채워 넣기
	if (!SpirteNormal) SpirteNormal = Fallback;
	if (!SpirteCombat) SpirteCombat = Fallback;
	if (!SpirteSelected) SpirteSelected = Fallback;
	if (!SpirteDead)   SpirteDead = Fallback;
}

void UIndicatorSpriteComponent::UpdateSpriteForState ()
{
	InitializeSpritesIfNeeded ();

	UPaperSprite* NewSprite = nullptr;

	switch (CurrentState)
	{
	case EIndicatorSpriteState::Normal:
		NewSprite = SpirteNormal;
		break;
	case EIndicatorSpriteState::Combat:
		NewSprite = SpirteCombat;
		break;
	case EIndicatorSpriteState::Selected:
		NewSprite = SpirteSelected;
		break;
	case EIndicatorSpriteState::Dead:
		NewSprite = SpirteDead;
		break;
	default:
		break;
	}

	// 데이터가 하나도 없을 시 방어 코드
	if (!NewSprite)
	{
		UE_LOG ( LogTemp , Warning ,
			TEXT ( "IndicatorSpriteComponent: %s 상태 %d에 사용할 Sprite가 없습니다." ) ,
			*GetOwner ()->GetName () ,
			static_cast<int32>(CurrentState) );
		return;
	}

	SetSprite ( NewSprite );
}

void UIndicatorSpriteComponent::UpdateGlow ( float DeltaTime )
{
	if (!DynamicMID)
	{
		return;
	}

	GlowTime += DeltaTime * GlowSpeed;

	const float Sine = 0.5f + 0.5f * FMath::Sin ( GlowTime );     // Sine Range : 0 ~ 1
	const float Glow = FMath::Lerp ( GlowMin , GlowMax , Sine );  // GlowMin ~ GlowMax

	DynamicMID->SetScalarParameterValue ( TEXT ( "Glow" ) , Glow );
}
