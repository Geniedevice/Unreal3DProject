// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RPGGunAbility.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API URPGGunAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    URPGGunAbility();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // 입력을 뗐을 때 호출됨 (AbilitySystemComponent의 Native 기능)
    virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    UFUNCTION()
    void StartFireLoop();

    void ExecuteFire(class AGunWeapon* Gun);

    UFUNCTION()
    void OnFireDelayFinished();

    UFUNCTION()
    void OnInputReleasedFromTask(float TimeHeld);

    UPROPERTY()
    TObjectPtr<class AGunWeapon> CachedGun;

    UPROPERTY(EditAnywhere, Category = "Effects")
    TObjectPtr<UAnimMontage> ShootMontage;

    FTimerHandle FireTimerHandle;
    bool bInputHeld = false;
};
