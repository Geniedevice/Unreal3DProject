// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Attribute/RPGAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Entity/Attribute/RPGGameplayTags.h"
#include "Net/UnrealNetwork.h" 

URPGAttributeSet::URPGAttributeSet() : Health(100.f), MaxHealth(100.f), AttackPower(10.f), Damage(10.f)
{

}

void URPGAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, CriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URPGAttributeSet, CriticalMultiplier, COND_None, REPNOTIFY_Always);
}

void URPGAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 체력이나 최대 체력이 바뀔 때 값 범위 제한
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void URPGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Damage 메타 어트리뷰트가 변경되었을 때만 실행
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		//  기초 데이터 추출 및 초기화
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0.f)
		{
			FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
			UAbilitySystemComponent* SourceASC = Context.GetInstigatorAbilitySystemComponent();

			float FinalDamage = FMath::Max(LocalDamageDone - GetArmor(), 1.0f);

			const float CritChance = GetCriticalChance();
			const float CritMultiplier = GetCriticalMultiplier();

			bool bIsCritical = (FMath::FRand() <= CritChance);

			if (bIsCritical)
			{
				FinalDamage *= CritMultiplier;
				UE_LOG(LogTemp, Warning, TEXT("Critical Hit! Damage: %f"), FinalDamage);
			}

			//GameplayCue 전용 파라미터 생성 및 설정
			FGameplayCueParameters Params;
			Params.RawMagnitude = FinalDamage; // 데미지 양 수치
			Params.Location = Data.Target.GetAvatarActor()->GetActorLocation(); // 피격 위치
			Params.Instigator = Context.GetInstigator(); // 공격자
			Params.EffectContext = Context; // 컨텍스트

			Params.AbilityLevel = bIsCritical ? 1 : 0;

			// 6. GameplayCue 실행
			//Data.Target.ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(FName("GameplayCue.Character.DamageText")), Params);
			Data.Target.ExecuteGameplayCue(FRPGGameplayTags::Get().GameplayCue_Character_DamageText, Params);

			// 7. 최종 체력 반영
			const float NewHealth = FMath::Clamp(GetHealth() - FinalDamage, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);

			// 디버그 출력
			UE_LOG(LogTemp, Log, TEXT("Final Applied Damage: %f | Current Health: %f"), FinalDamage, NewHealth);
		}
	}
}

// Armor 값이 서버에서 클라이언트로 복제될 때 실행됩니다.
void URPGAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
    // GAS 내부의 리플리케이션 시스템에 알림
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Armor, OldValue);
}

void URPGAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, Health, OldValue);
}

void URPGAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, MaxHealth, OldValue);
}

void URPGAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, AttackPower, OldValue);
}

// CriticalChance 값이 복제될 때 실행됩니다.
void URPGAttributeSet::OnRep_CriticalChance(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, CriticalChance, OldValue);
}

// CriticalMultiplier 값이 복제될 때 실행됩니다.
void URPGAttributeSet::OnRep_CriticalMultiplier(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(URPGAttributeSet, CriticalMultiplier, OldValue);
}