// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "RPGAttributeSet.generated.h"

// Accessor 매크로 정의
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class ACTIONPROJECT_API URPGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	URPGAttributeSet();

	// 네트워크 복제를 위한 필수 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- 체력 관련 ---
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, MaxHealth)

	// --- 공격/방어 관련 ---
	// 공격력도 멀티플레이어에서 동기화되도록 Replicated 추가 권장
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, AttackPower)

	// 데미지는 '메타 어트리뷰트'이므로 서버에서만 계산하고 복제(Replicate)하지 않습니다.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, Damage)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, Armor)

	// --- 치명타 관련 ---
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CriticalChance)
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, CriticalChance)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_CriticalMultiplier)
	FGameplayAttributeData CriticalMultiplier;
	ATTRIBUTE_ACCESSORS(URPGAttributeSet, CriticalMultiplier)

	// 수치 제한(Clamping) 등을 처리
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	// 이펙트 적용 후 실제 로직(데미지 계산 등) 처리
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	// OnRep 함수는 반드시 GAMEPLAYATTRIBUTE_REPNOTIFY 매크로를 포함해야 합니다.
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_AttackPower(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_CriticalChance(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_CriticalMultiplier(const FGameplayAttributeData& OldValue);
};
