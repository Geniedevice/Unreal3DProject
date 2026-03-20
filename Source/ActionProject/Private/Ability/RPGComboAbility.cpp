// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/RPGComboAbility.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include <Entity/Character/RPGPlayerCharacter.h>

URPGComboAbility::URPGComboAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	CurrentComboIndex = 1;
}

void URPGComboAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ARPGPlayerCharacter* RPGChar = Cast<ARPGPlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		RPGChar->bIsAttacking = true;
	}

	// 1. 몽타주 실행 (첫 섹션 Attack_1)
	UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), ComboMontage, 1.f, TEXT("Attack_1"));
	// 몽타주가 완전히 끝나면 어빌리티 종료
	MontageTask->OnCompleted.AddDynamic(this, &URPGComboAbility::K2_EndAbility);
	MontageTask->OnInterrupted.AddDynamic(this, &URPGComboAbility::K2_EndAbility);
	MontageTask->OnCancelled.AddDynamic(this, &URPGComboAbility::K2_EndAbility);
	MontageTask->ReadyForActivation();

	// 2. 콤보 체크 이벤트 대기 (InputTask는 지우고 이것만 남깁니다)
	// OnlyTriggerOnce 를 false로 해야 1타, 2타 노티파이를 계속 받습니다.
	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ComboEventTag, nullptr, false, true);
	EventTask->EventReceived.AddDynamic(this, &URPGComboAbility::OnComboCheckEventReceived);
	EventTask->ReadyForActivation();
}

void URPGComboAbility::SetInputWindow(bool bIsOpen)
{
	bIsInputWindowOpen = bIsOpen;
}

void URPGComboAbility::OnComboCheckEventReceived(FGameplayEventData Payload)
{
	if (bIsComboReserved)
	{
		AdvanceCombo();
	}
}

void URPGComboAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (bIsInputWindowOpen)
	{
		bIsComboReserved = true;

		// 클라이언트(로컬 컨트롤)이고 서버가 아닐 때만 이벤트를 보냄
		if (ActorInfo && ActorInfo->IsLocallyControlled() && !ActorInfo->IsNetAuthority())
		{
			FGameplayEventData Payload;
			Payload.EventTag = ComboEventTag;
			SendGameplayEvent(ComboEventTag, Payload);
		}
	}
}

void URPGComboAbility::AdvanceCombo()
{
	if (CurrentComboIndex < MaxComboCount)
	{
		CurrentComboIndex++; // 1->2, 2->3, 3->4
	}
	else
	{
		CurrentComboIndex = 1; // 4->1 (루프)
	}

	bIsComboReserved = false;

	// 섹션 이름 생성
	FName NextSection = *FString::Printf(TEXT("Attack_%d"), CurrentComboIndex);

	UE_LOG(LogTemp, Log, TEXT("Looping Combo - Jumping to Section: %s"), *NextSection.ToString());

	MontageJumpToSection(NextSection);
}

void URPGComboAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 변수 초기화
	if (ARPGPlayerCharacter* RPGChar = Cast<ARPGPlayerCharacter>(GetAvatarActorFromActorInfo()))
	{
		RPGChar->bIsAttacking = false;
	}

	CurrentComboIndex = 1;
	bIsComboReserved = false;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}