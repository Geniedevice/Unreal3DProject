#include "Ability/RPGGunAbility.h"
#include "Entity/Attribute/RPGGameplayTags.h"
#include "Entity/Weapon/GunWeapon.h"
#include "Entity/Character/RPGCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "TimerManager.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"

URPGGunAbility::URPGGunAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void URPGGunAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    // 중요: 활성화 시점에 상태 초기화
    bInputHeld = true;

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    ARPGCharacter* Character = Cast<ARPGCharacter>(ActorInfo->AvatarActor.Get());
    if (!Character) return;

    CachedGun = Cast<AGunWeapon>(Character->GetCurrentWeapon());
    if (!CachedGun || !CachedGun->CanFire())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // Task 생성: 입력을 뗐을 때 bInputHeld를 false로 바꿈
    UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
    WaitInputReleaseTask->OnRelease.AddDynamic(this, &URPGGunAbility::OnInputReleasedFromTask);
    WaitInputReleaseTask->ReadyForActivation();

    // 루프 시작
    StartFireLoop();
}

void URPGGunAbility::StartFireLoop()
{
    // 1. 유효성 및 탄약 체크
    if (!CachedGun || !CachedGun->CanFire() || !bInputHeld)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
        return;
    }

    // 2. 발사 실행 및 애니메이션
    ExecuteFire(CachedGun);

    if (ShootMontage && CurrentActorInfo->AvatarActor.IsValid())
    {
        if (ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get()))
        {
            Character->PlayAnimMontage(ShootMontage);
        }
    }

    // 타이머를 '단발성(false)'으로 설정
    GetWorld()->GetTimerManager().SetTimer(
        FireTimerHandle,
        this,
        &URPGGunAbility::OnFireDelayFinished,
        CachedGun->FireRate,
        false // 반복(Loop) 기능을 끕니다!
    );
}

void URPGGunAbility::ExecuteFire(AGunWeapon* Gun)
{
    // 서버에서만 투사체 생성 및 탄약 소모
    if (!HasAuthority(&CurrentActivationInfo) || !Gun) return;

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
    if (!SourceASC) return;

    FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), Gun);

    FGameplayEffectSpecHandle NewHandle = SourceASC->MakeOutgoingSpec(Gun->WeaponData.DamageEffect, GetAbilityLevel(), ContextHandle);

    if (NewHandle.IsValid())
    {
        FGameplayTag DamageTag = FRPGGameplayTags::Get().Data_Damage;
        NewHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, Gun->WeaponData.BaseDamage);

        Gun->SpawnProjectile(NewHandle);
        Gun->ConsumeAmmo();
    }
}

void URPGGunAbility::OnFireDelayFinished()
{
    // 타이머가 끝난 시점에 버튼을 누르고 있다면 다시 루프 실행
    if (bInputHeld)
    {
        StartFireLoop();
    }
    else
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

// 태스크를 통해 입력 해제가 감지되었을 때
void URPGGunAbility::OnInputReleasedFromTask(float TimeHeld)
{
    bInputHeld = false;
}

// GAS 시스템에 의한 직접적인 호출 (보조용)
void URPGGunAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputReleased(Handle, ActorInfo, ActivationInfo);

    bInputHeld = false;
}

void URPGGunAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    // 종료 시 타이머 제거
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}