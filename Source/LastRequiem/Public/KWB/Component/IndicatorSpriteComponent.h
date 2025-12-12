// IndicatorSpriteComponent.h

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
#include "IndicatorSpriteComponent.generated.h"

class UPaperSprite;
class UMaterialInstanceDynamic;

// 미니맵 아이콘 상태
UENUM ()
enum class EIndicatorSpriteState : uint8
{
	Normal ,
	Combat ,
	Dead
};

UCLASS ( Blueprintable , ClassGroup = (Custom) , meta = (BlueprintSpawnableComponent) )
class LASTREQUIEM_API UIndicatorSpriteComponent : public UPaperSpriteComponent
{
	GENERATED_BODY ()

public:
	UIndicatorSpriteComponent ();

	virtual void BeginPlay () override;
	virtual void TickComponent ( float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction ) override;

	// --- 각 액터에서 사용할 API ---

	// 상태 변경 (드론/적군/유닛 공통 진입점)
	void SetIndicatorState ( EIndicatorSpriteState NewState );

	// Glow 시작 (DurationSeconds < 0 이면 무한, 양수면 해당 시간 후 자동 종료)
	void StartGlow ( float DurationSeconds = -1.0f );

	// Glow 강제 종료
	void StopGlow ();

	EIndicatorSpriteState GetIndicatorState () const { return CurrentState; }

protected:
	// Sprite 세팅 : 필요 없는 건 nullptr 로 두면 됨
	UPROPERTY ( EditAnywhere , Category = "Data|Sprite" )
	TObjectPtr<UPaperSprite> SpirteNormal = nullptr;

	UPROPERTY ( EditAnywhere , Category = "Data|Sprite" )
	TObjectPtr<UPaperSprite> SpirteCombat = nullptr;

	UPROPERTY ( EditAnywhere , Category = "Data|Sprite" )
	TObjectPtr<UPaperSprite> SpirteDead = nullptr;

	// 시작 상태 (드론은 Normal, 적군은 Normal, 유닛도 Normal 정도가 기본일 듯)
	UPROPERTY ( EditAnywhere , Category = "Data|Sprite" )
	EIndicatorSpriteState InitialState = EIndicatorSpriteState::Normal;

	// Glow 세팅
	UPROPERTY ( EditAnywhere , Category = "Data|Glow" )
	float GlowSpeed = 5.0f;

	UPROPERTY ( EditAnywhere , Category = "Data|Glow" )
	float GlowMin = 0.5f;

	UPROPERTY ( EditAnywhere , Category = "Data|Glow" )
	float GlowMax = 2.0f;

	// 위치 / 회전 / 스케일 기본값
	UPROPERTY ( EditAnywhere , Category = "Minimap|Transform" )
	FTransform DefaultRelativeTransform;

	// 자동으로 이 기본값을 적용할지 여부
	UPROPERTY ( EditAnywhere , Category = "Minimap|Transform" )
	bool bApplyDefaultTransform = true;

	virtual void OnRegister () override;

private:
	// 현재 상태
	EIndicatorSpriteState CurrentState;

	// Sprite 초기화 여부 (Fallback 로직 한 번만 돌리기 위함)
	bool bSpritesInitialized;

	// Glow용
	UPROPERTY ( Transient )
	UMaterialInstanceDynamic* DynamicMID;

	bool  bGlowActive;
	float GlowTime;

	FTimerHandle GlowTimer; // 일정 시간 후 Glow 끄기용

	// 내부 헬퍼 함수들
	void InitializeSpritesIfNeeded ();
	void UpdateSpriteForState ();
	void UpdateGlow ( float DeltaTime );

	void HandleGlowFinished ();
};
