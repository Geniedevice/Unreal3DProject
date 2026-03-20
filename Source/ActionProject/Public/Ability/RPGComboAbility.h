// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RPGComboAbility.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API URPGComboAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	URPGComboAbility();

	// GA가 실행될 때 호출
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void SetInputWindow(bool bIsOpen);

protected:
	// 몽타주 노티파이(신호)를 받았을 때 실행할 함수
	UFUNCTION()
	void OnComboCheckEventReceived(FGameplayEventData Payload);

	// 입력(클릭)을 받았을 때 실행할 함수
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// 다음 콤보로 넘어가는 핵심 로직
	void AdvanceCombo();

	// 종료 처리
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	/** 데이터 설정 (블루프린트에서 입력) **/
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TObjectPtr<UAnimMontage> ComboMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	int32 MaxComboCount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	FGameplayTag ComboEventTag; // 예: Event.Combo.Check

	/** 상태 관리 변수 **/
	int32 CurrentComboIndex;

	bool bIsComboReserved;

	bool bIsInputWindowOpen = false;
};
