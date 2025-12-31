// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/Unit/B_UnitGrenadier.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"

AB_UnitGrenadier::AB_UnitGrenadier ()
{
}

void AB_UnitGrenadier::OnAttackHit_Unit ()
{
	if (!CurrentAttackTarget) return;

	FVector ExplosionLocation = CurrentAttackTarget->GetActorLocation ();

	UGameplayStatics::ApplyRadialDamage (
		GetWorld () ,
		AttackDamage ,
		ExplosionLocation ,
		ExplosionRadius ,
		UDamageType::StaticClass () ,
		TArray<AActor*> () ,    // 무시할 액터
		this ,                  // 데미지 제공자 
		GetController () ,      // 가해자 컨트롤러
		true                    // bDoFullDamage (true면 거리 상관없이 풀데미지, false면 멀수록 약해짐)
	);

	// 2. 시각 효과 (이펙트 펑!)
	if (ExplosionEffect)
	{
		if (ExplosionEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation (
				GetWorld () ,
				ExplosionEffect ,      // 헤더에서 선언한 나이아가라 에셋 변수
				ExplosionLocation ,    // 터질 위치
				FRotator::ZeroRotator , // 회전 (보통 폭발은 회전 상관없으니 Zero)
				FVector ( 1.0f ) ,         // 크기 (스케일)
				true ,                  // bAutoDestroy (이펙트 끝나면 자동 삭제)
				true ,                  // bAutoActivate (생성 즉시 재생)
				ENCPoolMethod::None ,   // 풀링 방식 (보통 None)
				true                   // bPreCullCheck (화면 밖이면 재생 안 함)
			);
		}

		//DrawDebugSphere ( GetWorld () , ExplosionLocation , ExplosionRadius , 12 , FColor::Orange , false , 1.0f );

	}
}
