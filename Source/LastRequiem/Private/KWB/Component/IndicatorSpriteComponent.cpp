// IndicatorSpriteComponent.cpp

#include "KWB/Component/IndicatorSpriteComponent.h"
#include "PaperSprite.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "TimerManager.h"

UIndicatorSpriteComponent::UIndicatorSpriteComponent ()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Default Transform
	DefaultRelativeTransform = FTransform (
		FRotator ( -90.f , 0.f , 0.f ) ,
		FVector ( 0.f , 0.f , 300.f ) ,	// TEMP : 레벨 구성 완료 후 결정
		FVector ( 0.35f , 0.35f , 0.35f )   // TEMP : 알파 게임 플레이 후 결정
	);

	// 상태/Glow 기본값
	CurrentState = EIndicatorSpriteState::Normal;
	bSpritesInitialized = false;

	DynamicMID = nullptr;
	bGlowActive = false;
	GlowTime = 0.0f;

	//// 스프라이트 설정
	//SetCollisionEnabled ( ECollisionEnabled::NoCollision );
	//SetCastShadow ( false );
	//SetReceivesDecals ( false );

	//// TEMP : 디버그용으로 잠시 꺼둠
	////bVisibleInSceneCaptureOnly = true;	// SceneCapture에서만 표시
	//bHiddenInGame = false;				// TEMP : 게임 카메라에는 안 보이지만 SceneCapture에는 보이게 / 설정 바꿔가면서 더 편한 것으로 결정 예정
	//SetVisibility ( true , true );
}

void UIndicatorSpriteComponent::BeginPlay ()
{
	Super::BeginPlay ();

	// 머티리얼 Dynamic Instance 생성 (Glow 제어용)
	if (GetNumMaterials () > 0)
	{
		if (UMaterialInterface* BaseMat = GetMaterial ( 0 ))
		{
			DynamicMID = CreateDynamicMaterialInstance ( 0 , BaseMat );
		}
	}

	// 초기 상태로 스프라이트 설정
	CurrentState = InitialState;
	InitializeSpritesIfNeeded ();
	UpdateSpriteForState ();
}

void UIndicatorSpriteComponent::OnRegister ()
{
	Super::OnRegister ();

	if (bApplyDefaultTransform)
	{
		// Default Transform 세팅
		SetRelativeTransform ( DefaultRelativeTransform );
	}

	// SceneCapture 에서만 보이게 옵션이 켜져 있으면 적용
	if (bVisibleOnlyInSceneCapture)
	{
		bVisibleInSceneCaptureOnly = true;  // UPrimitiveComponent 플래그
		bHiddenInGame = false;
		SetVisibility ( true , true );
	}
}

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
	bGlowActive = true;
	GlowTime = 0.0f;

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

void UIndicatorSpriteComponent::HandleGlowFinished ()
{
	StopGlow ();
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
