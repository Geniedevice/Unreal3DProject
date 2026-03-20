// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "GameplayTagContainer.h"
#include "RPGItemTypes.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 기본 생성자 (초기값 설정 권장)
	FWeaponData();

	/** 무기 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName WeaponName = NAME_None;

	/** 실제로 스폰할 무기 액터 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class ARPGWeapon> WeaponClass = nullptr;

	/** 장착될 소켓 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName AttachSocketName = NAME_None;

	/** 기초 공격력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BaseDamage = 0.f;

	/** 인벤토리 아이콘 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TObjectPtr<UTexture2D> WeaponIcon = nullptr;

	/** * [핵심] 상체 포즈 및 무기별 애니메이션 로직을 담당하는 레이어
	 * 이제 이 안에서 공격 몽타주 호출 인터페이스도 처리하게 됩니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSubclassOf<class UAnimInstance> WeaponAnimLayer;

	/** 이 무기를 들었을 때 '허용되는' 공격 어빌리티 (GA) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<class UGameplayAbility> AttackAbility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DamageEffect;

	/** 무기 식별용 태그 (예: Weapon.Melee.Sword) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FGameplayTag WeaponTypeTag;
};

USTRUCT(BlueprintType)
struct FCharacterAttributeRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 매칭용 태그
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag CharacterTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UGameplayEffect> DefaultAttributeGE;
};

UENUM(BlueprintType)
enum class ERPGAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	Attack, 
	Dash,// 공격은 3번(또는 원하는 숫자)으로 정의
};