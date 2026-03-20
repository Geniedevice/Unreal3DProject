// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/ANS_WeaponCollision.h"

#include "Entity/Character/RPGPlayerCharacter.h"
#include "Entity/Weapon/SwordWeapon.h"

// 1. 구간 시작 시 호출 (판정 초기화)
void UANS_WeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (ARPGPlayerCharacter* Player = Cast<ARPGPlayerCharacter>(MeshComp->GetOwner()))
	{
		// 캐릭터가 현재 들고 있는 무기를 가져와서 트레이스 시작
		if (ASwordWeapon* Sword = Cast<ASwordWeapon>(Player->GetCurrentWeapon()))
		{
			Sword->StartWeaponTrace();
		}
	}
}

// 2. 구간 진행 중 매 프레임 호출 (실제 판정 수행)
void UANS_WeaponCollision::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (ARPGPlayerCharacter* Player = Cast<ARPGPlayerCharacter>(MeshComp->GetOwner()))
	{
		if (ASwordWeapon* Sword = Cast<ASwordWeapon>(Player->GetCurrentWeapon()))
		{
			// 무기 클래스의 ExecutionTrace를 호출하여 매 프레임 충돌 체크
			Sword->ExecutionTrace();
		}
	}
}

// 3. 구간 종료 시 호출 (판정 종료)
void UANS_WeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ARPGPlayerCharacter* Player = Cast<ARPGPlayerCharacter>(MeshComp->GetOwner()))
	{
		if (ASwordWeapon* Sword = Cast<ASwordWeapon>(Player->GetCurrentWeapon()))
		{
			Sword->EndWeaponTrace();
		}
	}
}