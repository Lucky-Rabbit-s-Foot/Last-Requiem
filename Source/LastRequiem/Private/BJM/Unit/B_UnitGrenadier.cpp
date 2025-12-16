// Fill out your copyright notice in the Description page of Project Settings.


#include "BJM/Unit/B_UnitGrenadier.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

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
		UGameplayStatics::SpawnEmitterAtLocation (
			GetWorld () ,
			ExplosionEffect ,
			ExplosionLocation ,
			FRotator::ZeroRotator ,
			FVector ( 1.0f ) // 크기
		);
	}

	DrawDebugSphere ( GetWorld () , ExplosionLocation , ExplosionRadius , 12 , FColor::Orange , false , 1.0f );

	//UE_LOG ( LogTemp , Warning , TEXT ( "범위 공격 쾅!!!" ) );
}
