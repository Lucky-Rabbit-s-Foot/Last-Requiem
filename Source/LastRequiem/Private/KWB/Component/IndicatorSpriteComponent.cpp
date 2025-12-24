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
		FVector ( 0.f , 0.f , 4000.f ) ,	// TEMP : 레벨 구성 완료 후 결정 / 3000.0f로 테스트
		FVector ( 1.0f , 1.0f , 1.0f )   // TEMP : 알파 게임 플레이 후 결정 !!사이즈 변경 시 W_MapWidget에서 UnitSelectionRadius 값 변경할 것!!
	);

	// 상태/Glow 기본값
	CurrentState = EIndicatorSpriteState::Normal;
	bSpritesInitialized = false;

	DynamicMID = nullptr;
	CurrentGlowMode = EIndicatorGlowMode::None;
	bGlowActive = false;
	GlowTime = 0.0f;

	bCachedBaseColor = false;
	BaseSpriteColor = FLinearColor::White;

	// 스프라이트 설정
	SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	SetCastShadow ( false );
	SetReceivesDecals ( false );

	// SceneCapture에서만 보이게 설정
	bVisibleInSceneCaptureOnly = true;	// SceneCapture에서만 표시
	bHiddenInGame = false;
	SetVisibility ( true , false );	// propagate 끔 => true -> false : P_Obstacle 디버그
}

void UIndicatorSpriteComponent::BeginPlay ()
{
	Super::BeginPlay ();

	// 런타임 보정 1회
	ApplyDefaultTransformIfNeeded ( false );

	// 초기 상태로 스프라이트 설정
	CurrentState = InitialState;
	InitializeSpritesIfNeeded ();
	UpdateSpriteForState ();

	// 초기 색 캐시(StopGlow 시 원상복구 용도)
	if (!bCachedBaseColor)
	{
		BaseSpriteColor = GetSpriteColor ();
		bCachedBaseColor = true;
	}

	// 스프라이트가 세팅된 뒤 MID 생성이 더 안전함
	EnsureDynamicMaterial ( false );

	// 초기 상태에 맞게 자동 Glow 적용
	ApplyAutoGlowByState ();
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

	// 에디터 프리뷰에서도 색 캐시는 잡아둠(원복용)
	if (!bCachedBaseColor)
	{
		BaseSpriteColor = GetSpriteColor ();
		bCachedBaseColor = true;
	}

	// 스프라이트/머티리얼이 바뀌었을 수 있으니 MID는 한번 더 안전 보정
	EnsureDynamicMaterial ( false );

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
void UIndicatorSpriteComponent::PostEditChangeProperty ( FPropertyChangedEvent& PropertyChangedEvent )
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

	EnsureDynamicMaterial ( false );
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

	// 상태 기반 자동 Glow On/Off
	ApplyAutoGlowByState ();
}

void UIndicatorSpriteComponent::ApplyAutoGlowByState ()
{
	// 스프라이트가 꺼져있으면 굳이 Glow를 켤 필요 없음
	if (!bSpriteOn)
	{
		StopGlow ();
		return;
	}

	switch (CurrentState)
	{
	case EIndicatorSpriteState::Combat:
		// Combat은 Selected보다 빠른 속도 + 색 전환 파란 and 빨간
		StartGlowByMode ( EIndicatorGlowMode::Combat , -1.0f );
		break;

	case EIndicatorSpriteState::Selected:
		// Selected는 적당한 속도로 밝기만 반짝(색은 기본 유지)
		StartGlowByMode ( EIndicatorGlowMode::Selected , -1.0f );
		break;

	case EIndicatorSpriteState::Normal:
	case EIndicatorSpriteState::Dead:
	default:
		StopGlow ();
		break;
	}
}

void UIndicatorSpriteComponent::StartGlow ( float DurationSeconds )
{
	// 수동 호출 시에도 "현재 상태"에 맞는 프로파일로 동작하도록 처리
	if (CurrentState == EIndicatorSpriteState::Combat)
	{
		StartGlowByMode ( EIndicatorGlowMode::Combat , DurationSeconds );
	}
	else if (CurrentState == EIndicatorSpriteState::Selected)
	{
		StartGlowByMode ( EIndicatorGlowMode::Selected , DurationSeconds );
	}
	else
	{
		// Normal or Dead 상태에서 호출 -> 테스트 후 결정
		StartGlowByMode ( EIndicatorGlowMode::None , DurationSeconds );
	}
}

void UIndicatorSpriteComponent::StopGlow ()
{
	if (IsTemplate ())
	{
		return;
	}

	bGlowActive = false;
	CurrentGlowMode = EIndicatorGlowMode::None;

	// Tick 끄기 (Glow만 Tick을 쓰는 구조라면 OFF가 성능에 좋음)
	PrimaryComponentTick.SetTickFunctionEnable ( false );

	// 머티리얼 파라미터 원복
	if (DynamicMID)
	{
		DynamicMID->SetScalarParameterValue ( GlowScalarParamName , 1.0f );
	}

	// 색 원복(Combat에서 빨강/파랑으로 바꿨던 걸 되돌림)
	if (bCachedBaseColor)
	{
		SetSpriteColor ( BaseSpriteColor );
	}
	else
	{
		SetSpriteColor ( FLinearColor::White );
	}

	if (UWorld* World = GetWorld ())
	{
		World->GetTimerManager ().ClearTimer ( GlowTimer );
	}
}

void UIndicatorSpriteComponent::StartGlowByMode ( EIndicatorGlowMode Mode , float DurationSeconds )
{
	if (IsTemplate ())
	{
		return;
	}

	CurrentGlowMode = Mode;
	bGlowActive = (Mode != EIndicatorGlowMode::None);
	GlowTime = 0.0f;

	// 타이머 리셋(모드 변경 시 남은 타이머가 있으면 꼬일 수 있음)
	if (UWorld* World = GetWorld ())
	{
		World->GetTimerManager ().ClearTimer ( GlowTimer );
	}

	// 스프라이트/머티리얼이 바뀐 상태일 수 있으니 MID를 안전 보정
	EnsureDynamicMaterial ( false );

	// Tick 켜주기
	PrimaryComponentTick.SetTickFunctionEnable ( bGlowActive );

	// DurationSeconds > 0 이면 자동 종료
	if (bGlowActive && DurationSeconds > 0.0f)
	{
		if (UWorld* World = GetWorld ())
		{
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
	else if (bSpriteOn)
	{
		// 다시 켜면 현재 상태 기준으로 자동 Glow 다시 적용
		ApplyAutoGlowByState ();
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

void UIndicatorSpriteComponent::EnsureDynamicMaterial ( bool bForceRecreate )
{
	if (GetNumMaterials () <= 0)
	{
		DynamicMID = nullptr;
		return;
	}

	UMaterialInterface* Mat0 = GetMaterial ( 0 );
	if (!Mat0)
	{
		DynamicMID = nullptr;
		return;
	}

	// 이미 슬롯에 MID가 있으면 그걸 잡는다
	if (UMaterialInstanceDynamic* MID0 = Cast<UMaterialInstanceDynamic> ( Mat0 ))
	{
		DynamicMID = MID0;
		return;
	}

	// 슬롯이 MID가 아니라면, 현재 슬롯 머티리얼을 베이스로 MID를 "다시" 만들어 슬롯에 꽂는다
	DynamicMID = CreateDynamicMaterialInstance ( 0 , Mat0 );
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
	if (!SpirteNormal)	SpirteNormal = Fallback;
	if (!SpirteCombat)	SpirteCombat = Fallback;
	if (!SpirteSelected)SpirteSelected = Fallback;
	if (!SpirteDead)	SpirteDead = Fallback;
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

	// 스프라이트가 바뀌면 내부 머티리얼 세팅도 바뀔 수 있으니 MID 다시 확보
	EnsureDynamicMaterial (); // false 인자 전달 -> 인자 전달 X로 수정
}

void UIndicatorSpriteComponent::UpdateGlow ( float DeltaTime )
{
	if (!DynamicMID || CurrentGlowMode == EIndicatorGlowMode::None)
	{
		return;
	}

	// ---- 모드별 파라미터 선택 ----
	float UseSpeed = GlowSpeed;
	float UseMin = GlowMin;
	float UseMax = GlowMax;

	const bool bAnimateColor =
		(CurrentGlowMode == EIndicatorGlowMode::Combat); // Combat만 색 변화

	if (CurrentGlowMode == EIndicatorGlowMode::Combat)
	{
		UseSpeed = CombatGlowSpeed;
		UseMin = CombatGlowMin;
		UseMax = CombatGlowMax;
	}

	// ---- 밝기 펄스 ----
	GlowTime += DeltaTime * UseSpeed;

	const float Sine01 = 0.5f + 0.5f * FMath::Sin ( GlowTime ); // 0~1
	const float Glow = FMath::Lerp ( UseMin , UseMax , Sine01 );

	// 머티리얼에 "Glow" 스칼라가 있어야 실제로 밝기 변화가 보임
	DynamicMID->SetScalarParameterValue ( GlowScalarParamName , Glow );

	// ---- 색 변화(Combat 전용) ----
	if (bAnimateColor)
	{
		// 파란 <-> 빨간 전환이 더 자연스럽게 보이도록 HSV 기반 Lerp 권장
		const FLinearColor Mixed = FLinearColor::LerpUsingHSV ( CombatColorA_Blue , CombatColorB_Red , Sine01 );
		SetSpriteColor ( Mixed );
	}
	else
	{
		// Selected는 "빛만" 반짝 => 색은 원래대로 유지
		if (bCachedBaseColor)
		{
			SetSpriteColor ( BaseSpriteColor );
		}
	}
}
