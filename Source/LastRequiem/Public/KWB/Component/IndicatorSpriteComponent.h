// IndicatorSpriteComponent.h

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "IndicatorSpriteComponent.generated.h"

class UPaperSprite;
class UMaterialInstanceDynamic;

#if WITH_EDITOR
struct FPropertyChangedEvent;
#endif

// 미니맵 아이콘 상태
UENUM ()
enum class EIndicatorSpriteState : uint8
{
	Normal ,
	Combat ,
	Selected ,
	Dead
};

UENUM()
enum class EIndicatorGlowMode : uint8
{
	None,
	Selected,
	Combat
};

UCLASS ( Blueprintable , ClassGroup = (Custom) , meta = (BlueprintSpawnableComponent) )
class LASTREQUIEM_API UIndicatorSpriteComponent : public UPaperSpriteComponent
{
	GENERATED_BODY ()

public:
	UIndicatorSpriteComponent ();

	virtual void BeginPlay () override;
	virtual void TickComponent ( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction ) override;
	virtual void OnRegister () override;

	// Default Transform을 OnRegister에서 직접 만지지 않기 위한 보조 훅
	virtual void OnComponentCreated () override;
	virtual void PostLoad () override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty ( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif

	// --- 각 액터에서 사용할 API ---

	// 상태 변경 (드론/적군/유닛 공통 진입점)
	UFUNCTION ( BlueprintCallable , Category = "Indicator" )
	void SetIndicatorState ( EIndicatorSpriteState NewState );

	// Glow 시작 (DurationSeconds < 0 이면 무한, 양수면 해당 시간 후 자동 종료)
	UFUNCTION ( BlueprintCallable , Category = "Indicator" )
	void StartGlow ( float DurationSeconds = -1.0f );

	// Glow 강제 종료
	UFUNCTION ( BlueprintCallable , Category = "Indicator" )
	void StopGlow ();

	UFUNCTION ( BlueprintCallable , Category = "Indicator" )
	void SetSpriteOnOff ( bool bOn , bool bStopGlowWhenOff = true );

	UFUNCTION ( BlueprintCallable , Category = "Indicator" )
	void ToggleSpriteOnOff ();

	FORCEINLINE EIndicatorSpriteState GetIndicatorState () const { return CurrentState; }


protected:
	// Sprite 세팅 : 필요 없는 건 nullptr 로 두면 됨
	UPROPERTY ( EditAnywhere , Category = "Data|Sprite" )
	TObjectPtr<UPaperSprite> SpirteNormal = nullptr;

	UPROPERTY ( EditAnywhere , Category = "Data|Sprite" )
	TObjectPtr<UPaperSprite> SpirteCombat = nullptr;

	UPROPERTY ( EditAnywhere , Category = "Data|Sprite" )
	TObjectPtr<UPaperSprite> SpirteSelected = nullptr;

	UPROPERTY ( EditAnywhere , Category = "Data|Sprite" )
	TObjectPtr<UPaperSprite> SpirteDead = nullptr;

	// 시작 상태 (모든 액터 기본 Normal로 세팅)
	UPROPERTY ( EditAnywhere , Category = "Data|Sprite" )
	EIndicatorSpriteState InitialState = EIndicatorSpriteState::Normal;

	// Glow 세팅 || Selected Glow로 기능 -> 이름은 원래 이름 사용
	UPROPERTY ( EditAnywhere , Category = "Data|Glow|Selected" )
	float GlowSpeed = 5.0f;

	UPROPERTY ( EditAnywhere , Category = "Data|Glow|Selected" )
	float GlowMin = 0.5f;

	UPROPERTY ( EditAnywhere , Category = "Data|Glow|Selected" )
	float GlowMax = 5.0f;

	// Combat Glow 세팅
	UPROPERTY ( EditAnywhere , Category = "Data|Glow|Combat" )
	float CombatGlowSpeed = 12.0f;

	UPROPERTY ( EditAnywhere , Category = "Data|Glow|Combat" )
	float CombatGlowMin = 0.8f;

	UPROPERTY ( EditAnywhere , Category = "Data|Glow|Combat" )
	float CombatGlowMax = 12.0f;

	// Combat에서 색을 파란 <-> 빨간으로 변환하고 싶을 때 사용할 색
	UPROPERTY ( EditAnywhere , Category = "Data|Glow|Combat" )
	FLinearColor CombatColorA_Blue = FLinearColor ( 0.10f , 0.45f , 1.00f , 1.00f );

	UPROPERTY ( EditAnywhere , Category = "Data|Glow|Combat" )
	FLinearColor CombatColorB_Red = FLinearColor ( 1.00f , 0.10f , 0.10f , 1.00f );

	// 머티리얼 Scalar 파라미터 이름(기본 "Glow")
	UPROPERTY ( EditAnywhere , Category = "Data|Glow" )
	FName GlowScalarParamName = TEXT ( "Glow" );

	// 위치 / 회전 / 스케일 기본값
	UPROPERTY ( EditAnywhere , Category = "Minimap|Transform" )
	FTransform DefaultRelativeTransform;

	// 자동으로 이 기본값을 적용할지 여부
	UPROPERTY ( EditAnywhere , Category = "Minimap|Transform" )
	bool bApplyDefaultTransform = true;

	// 에디터/게임 뷰포트에는 보이지 않고 SceneCapture 에서만 보이도록 할지 여부
	UPROPERTY ( EditAnywhere , Category = "Indicator|Visibility" )
	bool bVisibleOnlyInSceneCapture = true;

private:
	// 현재 상태
	EIndicatorSpriteState CurrentState = EIndicatorSpriteState::Normal;

	// 스프라이트 온오프 플래그
	bool bSpriteOn = true;

	// Sprite 초기화 여부 (Fallback 로직 한 번만 돌리기 위함)
	bool bSpritesInitialized = false;

	// Glow용 MID
	UPROPERTY ( Transient )
	UMaterialInstanceDynamic* DynamicMID = nullptr;

	// 현재 Glow 모드(Selected/Combat/None)
	EIndicatorGlowMode CurrentGlowMode = EIndicatorGlowMode::None;

	bool  bGlowActive = false;
	float GlowTime = 0.0f;

	// StopGlow 시 원래 색으로 되돌리기 위한 캐시
	bool bCachedBaseColor = false;
	FLinearColor BaseSpriteColor = FLinearColor::White;

	// 일정 시간 후 Glow 끄기용
	FTimerHandle GlowTimer;

	// OnRegister 재진입 가드 (스택 오버플로우 방지)
	bool bInOnRegister = false;

	// Default Transform 1회 적용 플래그
	bool bDefaultTransformApplied = false;

	// 내부 헬퍼 함수들
	void InitializeSpritesIfNeeded ();
	void UpdateSpriteForState ();

	// EIndicatorGlowMode에 따라서 자동으로 Glow모드 결정 And On/Off
	void ApplyAutoGlowByState ();

	// Glow 모드로 시작(Selected/Combat), DurationSeconds < 0 이면 무한
	void StartGlowByMode ( EIndicatorGlowMode Mode , float DurationSeconds );

	void UpdateGlow ( float DeltaTime );
	void HandleGlowFinished ();

	// 옵션에 맞게 가시성/SceneCapture 플래그 동기화
	void ApplyVisibilityRules ();

	// Glow용 MID 안전 생성
	void EnsureDynamicMaterial ( bool bForceRecreate = false );

	// Default Transform 안전 적용 (강제/비강제)
	void ApplyDefaultTransformIfNeeded ( bool bForce );

	// 다음 틱 콜백(=OnRegister 스택 밖)에서 Transform 적용
	void ApplyDefaultTransformDeferred ();
};
